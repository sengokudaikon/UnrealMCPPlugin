#include "Services/BlueprintService.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Editor.h"
#include "EditorAssetLibrary.h"
#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ObjectTools.h"

namespace UnrealMCP {
	auto FBlueprintService::SpawnActorBlueprint(const FBlueprintSpawnParams& Params) -> TResult<AActor*> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<AActor*>::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (Params.ActorName.IsEmpty()) {
			return TResult<AActor*>::Failure(TEXT("Actor name cannot be empty"));
		}

		// Find blueprint
		const UBlueprint* Blueprint = FCommonUtils::FindBlueprint(Params.BlueprintName);
		if (!Blueprint) {
			return TResult<AActor*>::Failure(
				FString::Printf(TEXT("Blueprint not found: %s"), *Params.BlueprintName)
			);
		}

		// Get editor world
		UWorld* World = GEditor->GetEditorWorldContext().World();
		if (!World) {
			UE_LOG(LogTemp, Error, TEXT("SpawnActorBlueprint: Failed to get editor world context"));
			return TResult<AActor*>::Failure(TEXT("Failed to get editor world"));
		}

		// Validate blueprint status
		if (Blueprint->Status != BS_UpToDate) {
			UE_LOG(
				LogTemp,
				Error,
				TEXT("SpawnActorBlueprint: Blueprint '%s' is not up to date (Status: %d)"),
				*Blueprint->GetName(),
				static_cast<int32>(Blueprint->Status)
			);

			FString StatusMessage;
			switch (Blueprint->Status) {
				case BS_Unknown:
					StatusMessage = TEXT("Unknown - blueprint may be corrupted");
					break;
				case BS_Dirty:
					StatusMessage = TEXT("Dirty - blueprint has unsaved changes");
					break;
				case BS_Error:
					StatusMessage = TEXT("Error - blueprint has compilation errors");
					break;
				case BS_BeingCreated:
					StatusMessage = TEXT("Being Created - blueprint is still being created");
					break;
				default:
					StatusMessage = FString::Printf(TEXT("Status %d"), static_cast<int32>(Blueprint->Status));
					break;
			}

			return TResult<AActor*>::Failure(
				FString::Printf(
					TEXT("Blueprint '%s' is not ready to spawn (Status: %s)"),
					*Params.BlueprintName,
					*StatusMessage)
			);
		}

		if (!Blueprint->GeneratedClass) {
			UE_LOG(
				LogTemp,
				Error,
				TEXT("SpawnActorBlueprint: Blueprint '%s' has no generated class - may not be compiled properly"),
				*Blueprint->GetName()
			);
			return TResult<AActor*>::Failure(
				FString::Printf(
					TEXT("Blueprint '%s' has no generated class - may not be compiled properly"),
					*Params.BlueprintName)
			);
		}

		if (!Blueprint->GeneratedClass->IsChildOf<AActor>()) {
			UE_LOG(
				LogTemp,
				Error,
				TEXT("SpawnActorBlueprint: Blueprint '%s' generated class is not a child of AActor"),
				*Blueprint->GetName()
			);
			return TResult<AActor*>::Failure(
				FString::Printf(TEXT("Blueprint '%s' is not an Actor-based blueprint"), *Params.BlueprintName)
			);
		}

		// Log complexity warnings
		bool bIsComplexBlueprint = false;
		FString ComplexityInfo;

		if (Blueprint->SimpleConstructionScript) {
			if (const int32 ComponentCount = Blueprint->SimpleConstructionScript->GetAllNodes().Num(); ComponentCount > 10) {
				bIsComplexBlueprint = true;
				ComplexityInfo = FString::Printf(TEXT("High component count: %d"), ComponentCount);
			}
		}

		if (Blueprint->Timelines.Num() > 0) {
			bIsComplexBlueprint = true;
			if (!ComplexityInfo.IsEmpty()) {
				ComplexityInfo += TEXT(", ");
			}
			ComplexityInfo += FString::Printf(TEXT("Timelines: %d"), Blueprint->Timelines.Num());
		}

		if (bIsComplexBlueprint) {
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("SpawnActorBlueprint: Blueprint '%s' appears complex (%s) - spawn may take longer"),
				*Blueprint->GetName(),
				*ComplexityInfo
			);
		}

		// Prepare spawn transform
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Params.Location.Get(FVector::ZeroVector));
		SpawnTransform.SetRotation(FQuat(Params.Rotation.Get(FRotator::ZeroRotator)));

		// Handle name conflicts
		FString FinalActorName = Params.ActorName;
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

		for (const AActor* ExistingActor : AllActors) {
			if (ExistingActor && ExistingActor->GetActorLabel().Equals(FinalActorName)) {
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("SpawnActorBlueprint: Actor name '%s' already exists, appending timestamp"),
					*FinalActorName
				);
				const FDateTime Now = FDateTime::Now();
				FinalActorName = FString::Printf(TEXT("%s_%lld"), *FinalActorName, Now.GetTicks());
				break;
			}
		}

		// Configure spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Name = FName(*FinalActorName);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("SpawnActorBlueprint: Attempting to spawn '%s' from blueprint '%s'"),
			*FinalActorName,
			*Blueprint->GetName()
		);

		// Spawn the actor
		AActor* NewActor = World->SpawnActor<AActor>(Blueprint->GeneratedClass, SpawnTransform, SpawnParams);

		if (!NewActor) {
			UE_LOG(LogTemp, Error, TEXT("SpawnActorBlueprint: Failed to spawn blueprint actor '%s'"), *FinalActorName);
			UE_LOG(LogTemp, Error, TEXT("  Blueprint: %s"), *Blueprint->GetName());
			UE_LOG(LogTemp, Error, TEXT("  Spawn Location: %s"), *SpawnTransform.GetLocation().ToString());
			UE_LOG(LogTemp, Error, TEXT("  Generated Class: %s"), *Blueprint->GeneratedClass->GetName());
			UE_LOG(LogTemp, Error, TEXT("  World Context: %s"), World ? *World->GetName() : TEXT("None"));

			if (Blueprint->SimpleConstructionScript) {
				UE_LOG(
					LogTemp,
					Error,
					TEXT("  SCS Components: %d"),
					Blueprint->SimpleConstructionScript->GetAllNodes().Num()
				);
			}
			UE_LOG(
				LogTemp,
				Error,
				TEXT("  Parent Class: %s"),
				Blueprint->ParentClass ? *Blueprint->ParentClass->GetName() : TEXT("None")
			);

			return TResult<AActor*>::Failure(
				FString::Printf(
					TEXT(
						"Failed to spawn blueprint actor '%s' - blueprint may have compilation errors or missing dependencies"),
					*Params.BlueprintName
				)
			);
		}

		// Set actor label
		NewActor->SetActorLabel(FinalActorName);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("SpawnActorBlueprint: Successfully spawned blueprint actor '%s' (Class: %s)"),
			*FinalActorName,
			*NewActor->GetClass()->GetName()
		);

		return TResult<AActor*>::Success(NewActor);
	}

	auto FBlueprintService::AddComponent(const FComponentParams& Params) -> TResult<UBlueprint*> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<UBlueprint*>::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (Params.ComponentName.IsEmpty()) {
			return TResult<UBlueprint*>::Failure(TEXT("Component name cannot be empty"));
		}
		if (Params.ComponentType.IsEmpty()) {
			return TResult<UBlueprint*>::Failure(TEXT("Component type cannot be empty"));
		}

		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(Params.BlueprintName);
		if (!Blueprint) {
			return TResult<UBlueprint*>::Failure(FString::Printf(TEXT("Blueprint not found: %s"), *Params.BlueprintName)
			);
		}

		const FString ValidationError = ValidateBlueprintForComponentOps(Blueprint);
		if (!ValidationError.IsEmpty()) {
			return TResult<UBlueprint*>::Failure(ValidationError);
		}

		UClass* ComponentClass = ResolveComponentClass(Params.ComponentType);
		if (!ComponentClass) {
			return TResult<UBlueprint*>::Failure(
				FString::Printf(TEXT("Unknown component type: %s"), *Params.ComponentType)
			);
		}

		USCS_Node* NewNode = Blueprint->SimpleConstructionScript->CreateNode(ComponentClass, *Params.ComponentName);
		if (!NewNode) {
			return TResult<UBlueprint*>::Failure(TEXT("Failed to create component node"));
		}
		
		if (USceneComponent* SceneComponent = Cast<USceneComponent>(NewNode->ComponentTemplate)) {
			if (Params.Location.IsSet()) {
				SceneComponent->SetRelativeLocation(Params.Location.GetValue());
			}
			if (Params.Rotation.IsSet()) {
				SceneComponent->SetRelativeRotation(Params.Rotation.GetValue());
			}
			if (Params.Scale.IsSet()) {
				SceneComponent->SetRelativeScale3D(Params.Scale.GetValue());
			}
		}

		// Apply static mesh if specified
		if (Params.MeshType.IsSet()) {
			if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(NewNode->ComponentTemplate); MeshComponent && !Params.MeshType.GetValue().IsEmpty()) {
				if (UStaticMesh* Mesh = Cast<UStaticMesh>(UEditorAssetLibrary::LoadAsset(Params.MeshType.GetValue()))) {
					MeshComponent->SetStaticMesh(Mesh);
				}
			}
		}

		if (Params.Properties.IsValid()) {
			for (const auto& PropertyPair : Params.Properties->Values) {
				if (FString ErrorMessage; !FCommonUtils::SetObjectProperty(
					NewNode->ComponentTemplate,
					PropertyPair.Key,
					PropertyPair.Value,
					ErrorMessage
				)) {
					UE_LOG(
						LogTemp,
						Warning,
						TEXT("AddComponent - Failed to set property %s: %s"),
						*PropertyPair.Key,
						*ErrorMessage
					);
				}
			}
		}

		Blueprint->SimpleConstructionScript->AddNode(NewNode);
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		UE_LOG(
			LogTemp,
			Display,
			TEXT("AddComponent - Added component %s of type %s to blueprint %s"),
			*Params.ComponentName,
			*Params.ComponentType,
			*Params.BlueprintName
		);

		return TResult<UBlueprint*>::Success(Blueprint);
	}

	auto FBlueprintService::SetComponentProperty(
		const FString& BlueprintName,
		const FString& ComponentName,
		const FPropertyParams& PropertyParams
	) -> FVoidResult {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (ComponentName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Component name cannot be empty"));
		}
		if (PropertyParams.PropertyName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Property name cannot be empty"));
		}

		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName));
		}

		if (const FString ValidationError = ValidateBlueprintForComponentOps(Blueprint); !ValidationError.IsEmpty()) {
			return FVoidResult::Failure(ValidationError);
		}

		const USCS_Node* ComponentNode = FindComponentNode(Blueprint, ComponentName);
		if (!ComponentNode) {
			return FVoidResult::Failure(FString::Printf(TEXT("Component not found: %s"), *ComponentName));
		}

		UObject* ComponentTemplate = ComponentNode->ComponentTemplate;
		if (!ComponentTemplate) {
			return FVoidResult::Failure(TEXT("Invalid component template"));
		}

		FString ErrorMessage;
		if (!FCommonUtils::SetObjectProperty(
			ComponentTemplate,
			PropertyParams.PropertyName,
			PropertyParams.PropertyValue,
			ErrorMessage
		)) {
			return FVoidResult::Failure(ErrorMessage);
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		UE_LOG(
			LogTemp,
			Display,
			TEXT("SetComponentProperty - Set property %s on component %s"),
			*PropertyParams.PropertyName,
			*ComponentName
		);

		return FVoidResult::Success();
	}

	auto FBlueprintService::SetPhysicsProperties(const FPhysicsParams& Params) -> FVoidResult {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (Params.ComponentName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Component name cannot be empty"));
		}

		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(Params.BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint not found: %s"), *Params.BlueprintName));
		}

		const FString ValidationError = ValidateBlueprintForComponentOps(Blueprint);
		if (!ValidationError.IsEmpty()) {
			return FVoidResult::Failure(ValidationError);
		}

		const USCS_Node* ComponentNode = FindComponentNode(Blueprint, Params.ComponentName);
		if (!ComponentNode) {
			return FVoidResult::Failure(FString::Printf(TEXT("Component not found: %s"), *Params.ComponentName));
		}

		UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(ComponentNode->ComponentTemplate);
		if (!PrimComponent) {
			return FVoidResult::Failure(TEXT("Component is not a primitive component"));
		}

		PrimComponent->SetSimulatePhysics(Params.bSimulatePhysics);
		PrimComponent->SetMassOverrideInKg(NAME_None, Params.Mass);
		PrimComponent->SetLinearDamping(Params.LinearDamping);
		PrimComponent->SetAngularDamping(Params.AngularDamping);
		PrimComponent->SetEnableGravity(Params.bEnableGravity);

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		UE_LOG(
			LogTemp,
			Display,
			TEXT("SetPhysicsProperties - Set physics on component %s: Simulate=%d, Mass=%f, LDamp=%f, ADamp=%f, "
				"Gravity=%d"),
			*Params.ComponentName,
			Params.bSimulatePhysics,
			Params.Mass,
			Params.LinearDamping,
			Params.AngularDamping,
			Params.bEnableGravity
		);

		return FVoidResult::Success();
	}

	auto FBlueprintService::SetStaticMeshProperties(
		const FString& BlueprintName,
		const FString& ComponentName,
		const FString& StaticMesh,
		const TOptional<FString>& Material
	) -> FVoidResult {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (ComponentName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Component name cannot be empty"));
		}
		if (StaticMesh.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Static mesh path cannot be empty"));
		}

		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName));
		}

		if (const FString ValidationError = ValidateBlueprintForComponentOps(Blueprint); !ValidationError.IsEmpty()) {
			return FVoidResult::Failure(ValidationError);
		}

		const USCS_Node* ComponentNode = FindComponentNode(Blueprint, ComponentName);
		if (!ComponentNode) {
			return FVoidResult::Failure(FString::Printf(TEXT("Component not found: %s"), *ComponentName));
		}

		UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(ComponentNode->ComponentTemplate);
		if (!MeshComponent) {
			return FVoidResult::Failure(TEXT("Component is not a static mesh component"));
		}

		if (!StaticMesh.IsEmpty()) {
			UStaticMesh* MeshAsset = Cast<UStaticMesh>(UEditorAssetLibrary::LoadAsset(StaticMesh));
			if (!MeshAsset) {
				return FVoidResult::Failure(FString::Printf(TEXT("Failed to load static mesh: %s"), *StaticMesh));
			}
			MeshComponent->SetStaticMesh(MeshAsset);
		}

		if (Material.IsSet() && !Material.GetValue().IsEmpty()) {
			UMaterialInterface* MaterialAsset =
				Cast<UMaterialInterface>(UEditorAssetLibrary::LoadAsset(Material.GetValue()));
			if (!MaterialAsset) {
				return FVoidResult::Failure(FString::Printf(TEXT("Failed to load material: %s"), *Material.GetValue()));
			}
			MeshComponent->SetMaterial(0, MaterialAsset);
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		UE_LOG(LogTemp, Display, TEXT("SetStaticMeshProperties - Updated mesh on component %s"), *ComponentName);

		return FVoidResult::Success();
	}

	auto FBlueprintService::SetBlueprintProperty(
		const FString& BlueprintName,
		const FPropertyParams& PropertyParams
	) -> FVoidResult {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (PropertyParams.PropertyName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Property name cannot be empty"));
		}

		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName));
		}

		UObject* DefaultObject = Blueprint->GeneratedClass->GetDefaultObject();
		if (!DefaultObject) {
			return FVoidResult::Failure(TEXT("Failed to get blueprint class default object"));
		}

		if (FString ErrorMessage; !FCommonUtils::SetObjectProperty(
			DefaultObject,
			PropertyParams.PropertyName,
			PropertyParams.PropertyValue,
			ErrorMessage
		)) {
			return FVoidResult::Failure(ErrorMessage);
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		UE_LOG(
			LogTemp,
			Display,
			TEXT("SetBlueprintProperty - Set property %s on blueprint %s"),
			*PropertyParams.PropertyName,
			*BlueprintName
		);

		return FVoidResult::Success();
	}

	auto FBlueprintService::SetPawnProperties(
		const FString& BlueprintName,
		const TSharedPtr<FJsonObject>& PropertyParams
	) -> FVoidResult {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (!PropertyParams.IsValid()) {
			return FVoidResult::Failure(TEXT("Invalid property parameters"));
		}

		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName));
		}

		UObject* DefaultObject = Blueprint->GeneratedClass->GetDefaultObject();
		if (!DefaultObject) {
			return FVoidResult::Failure(TEXT("Failed to get default object"));
		}

		bool bAnyPropertiesSet = false;

		if (PropertyParams->HasField(TEXT("auto_possess_player"))) {
			if (FString ErrorMessage; FCommonUtils::SetObjectProperty(
				DefaultObject,
				TEXT("AutoPossessPlayer"),
				PropertyParams->Values.FindRef(TEXT("auto_possess_player")),
				ErrorMessage
			)) {
				bAnyPropertiesSet = true;
			}
		}

		const TCHAR* ParamNames[] = {
			TEXT("use_controller_rotation_yaw"),
			TEXT("use_controller_rotation_pitch"),
			TEXT("use_controller_rotation_roll")
		};

		for (int32 i = 0; i < 3; ++i) {
			if (PropertyParams->HasField(ParamNames[i])) {
				const TCHAR* RotationProps[] = {
					TEXT("bUseControllerRotationYaw"), TEXT("bUseControllerRotationPitch"),
					TEXT("bUseControllerRotationRoll")
				};
				FString ErrorMessage;
				if (FCommonUtils::SetObjectProperty(
					DefaultObject,
					RotationProps[i],
					PropertyParams->Values.FindRef(ParamNames[i]),
					ErrorMessage
				)) {
					bAnyPropertiesSet = true;
				}
			}
		}

		if (PropertyParams->HasField(TEXT("can_be_damaged"))) {
			if (FString ErrorMessage; FCommonUtils::SetObjectProperty(
				DefaultObject,
				TEXT("bCanBeDamaged"),
				PropertyParams->Values.FindRef(TEXT("can_be_damaged")),
				ErrorMessage
			)) {
				bAnyPropertiesSet = true;
			}
		}

		if (!bAnyPropertiesSet) {
			return FVoidResult::Failure(TEXT("No pawn properties specified or all failed to set"));
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		UE_LOG(LogTemp, Display, TEXT("SetPawnProperties - Updated pawn properties on blueprint %s"), *BlueprintName);

		return FVoidResult::Success();
	}

	auto FBlueprintService::FindComponentNode(const UBlueprint* Blueprint, const FString& ComponentName) -> USCS_Node* {
		if (!Blueprint || !Blueprint->SimpleConstructionScript) {
			return nullptr;
		}

		for (USCS_Node* Node : Blueprint->SimpleConstructionScript->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == ComponentName) {
				return Node;
			}
		}

		return nullptr;
	}

	FString FBlueprintService::ValidateBlueprintForComponentOps(const UBlueprint* Blueprint) {
		if (!Blueprint) {
			return TEXT("Blueprint is invalid");
		}

		if (!Blueprint->SimpleConstructionScript) {
			return TEXT("Blueprint has no SimpleConstructionScript");
		}

		return FString();
	}

	UClass* FBlueprintService::ResolveComponentClass(const FString& ComponentType) {
		UClass* ComponentClass = FindFirstObject<UClass>(*ComponentType, EFindFirstObjectOptions::NativeFirst);
		if (ComponentClass && ComponentClass->IsChildOf(UActorComponent::StaticClass())) {
			return ComponentClass;
		}

		if (!ComponentType.EndsWith(TEXT("Component"))) {
			const FString WithSuffix = ComponentType + TEXT("Component");
			ComponentClass = FindFirstObject<UClass>(*WithSuffix, EFindFirstObjectOptions::NativeFirst);
			if (ComponentClass && ComponentClass->IsChildOf(UActorComponent::StaticClass())) {
				return ComponentClass;
			}
		}

		if (!ComponentType.StartsWith(TEXT("U"))) {
			const FString WithPrefix = TEXT("U") + ComponentType;
			ComponentClass = FindFirstObject<UClass>(*WithPrefix, EFindFirstObjectOptions::NativeFirst);
			if (ComponentClass && ComponentClass->IsChildOf(UActorComponent::StaticClass())) {
				return ComponentClass;
			}

			if (!ComponentType.EndsWith(TEXT("Component"))) {
				const FString WithBoth = TEXT("U") + ComponentType + TEXT("Component");
				ComponentClass = FindFirstObject<UClass>(*WithBoth, EFindFirstObjectOptions::NativeFirst);
				if (ComponentClass && ComponentClass->IsChildOf(UActorComponent::StaticClass())) {
					return ComponentClass;
				}
			}
		}

		return nullptr;
	}

	auto FBlueprintService::SetComponentTransform(const FComponentTransformParams& Params) -> TResult<FComponentTransformResult> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<FComponentTransformResult>::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (Params.ComponentName.IsEmpty()) {
			return TResult<FComponentTransformResult>::Failure(TEXT("Component name cannot be empty"));
		}

		// Find blueprint
		UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(Params.BlueprintName));
		if (!Blueprint) {
			return TResult<FComponentTransformResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *Params.BlueprintName)
			);
		}

		// Validate blueprint for component operations
		const FString ValidationError = ValidateBlueprintForComponentOps(Blueprint);
		if (!ValidationError.IsEmpty()) {
			return TResult<FComponentTransformResult>::Failure(ValidationError);
		}

		// Find component node
		USCS_Node* TargetNode = FindComponentNode(Blueprint, Params.ComponentName);
		if (!TargetNode || !TargetNode->ComponentTemplate) {
			return TResult<FComponentTransformResult>::Failure(
				FString::Printf(TEXT("Component '%s' not found in blueprint"), *Params.ComponentName)
			);
		}

		// Verify it's a scene component
		USceneComponent* SceneComp = Cast<USceneComponent>(TargetNode->ComponentTemplate);
		if (!SceneComp) {
			return TResult<FComponentTransformResult>::Failure(
				TEXT("Component is not a SceneComponent and doesn't have transform properties")
			);
		}

		bool bModified = false;

		// Apply transform changes
		if (Params.Location.IsSet()) {
			SceneComp->SetRelativeLocation(Params.Location.GetValue());
			bModified = true;
		}

		if (Params.Rotation.IsSet()) {
			SceneComp->SetRelativeRotation(Params.Rotation.GetValue());
			bModified = true;
		}

		if (Params.Scale.IsSet()) {
			SceneComp->SetRelativeScale3D(Params.Scale.GetValue());
			bModified = true;
		}

		if (!bModified) {
			return TResult<FComponentTransformResult>::Failure(
				TEXT("No valid transform parameters provided to modify")
			);
		}

		// Mark blueprint as modified and compile
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		// Return the updated transform
		FComponentTransformResult Result;
		Result.Location = SceneComp->GetRelativeLocation();
		Result.Rotation = SceneComp->GetRelativeRotation();
		Result.Scale = SceneComp->GetRelativeScale3D();

		return TResult<FComponentTransformResult>::Success(MoveTemp(Result));
	}

	auto FBlueprintService::DeleteBlueprint(const FDeleteBlueprintParams& Params) -> TResult<FDeleteBlueprintResult> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<FDeleteBlueprintResult>::Failure(TEXT("Blueprint name cannot be empty"));
		}

		// Find the blueprint
		const FString BlueprintPath = FBlueprintIntrospectionService::GetBlueprintPath(Params.BlueprintName);
		if (BlueprintPath.IsEmpty()) {
			return TResult<FDeleteBlueprintResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *Params.BlueprintName)
			);
		}

		UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *BlueprintPath);
		if (!Blueprint) {
			return TResult<FDeleteBlueprintResult>::Failure(
				FString::Printf(TEXT("Failed to load blueprint '%s'"), *Params.BlueprintName)
			);
		}

		// Get the package
		const UPackage* Package = Blueprint->GetOutermost();
		if (!Package) {
			return TResult<FDeleteBlueprintResult>::Failure(TEXT("Failed to get blueprint package"));
		}

		// Delete the asset
		TArray<UObject*> ObjectsToDelete;
		ObjectsToDelete.Add(Blueprint);

		const int32 NumDeleted = ObjectTools::DeleteObjects(ObjectsToDelete, false);

		if (NumDeleted > 0) {
			// Notify asset registry
			const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			AssetRegistryModule.Get().AssetDeleted(Blueprint);

			// Return success result
			FDeleteBlueprintResult Result;
			Result.DeletedPath = BlueprintPath;
			return TResult<FDeleteBlueprintResult>::Success(MoveTemp(Result));
		}

		return TResult<FDeleteBlueprintResult>::Failure(
			TEXT("Failed to delete blueprint - it may be in use or referenced elsewhere")
		);
	}

}
