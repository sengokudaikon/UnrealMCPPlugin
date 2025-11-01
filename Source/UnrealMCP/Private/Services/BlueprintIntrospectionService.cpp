#include "Services/BlueprintIntrospectionService.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/LightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/UObjectIterator.h"

namespace UnrealMCP {

	auto FBlueprintIntrospectionService::ListBlueprints(
		const FString& Path,
		const bool bRecursive,
		TArray<FString>& OutBlueprints
	) -> FVoidResult {
		OutBlueprints.Empty();

		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			"AssetRegistry");
		const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		FARFilter Filter;
		Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
		Filter.PackagePaths.Add(FName(*Path));
		Filter.bRecursivePaths = bRecursive;

		TArray<FAssetData> AssetDataList;
		AssetRegistry.GetAssets(Filter, AssetDataList);

		for (const FAssetData& AssetData : AssetDataList) {
			OutBlueprints.Add(AssetData.GetObjectPathString());
		}

		return FVoidResult::Success();
	}

	auto FBlueprintIntrospectionService::BlueprintExists(const FString& BlueprintName) -> bool {
		const UBlueprint* Blueprint = FindBlueprint(BlueprintName);
		return Blueprint != nullptr;
	}

	auto FBlueprintIntrospectionService::GetBlueprintInfo(
		const FString& BlueprintName,
		TMap<FString, FString>& OutInfo
	) -> FVoidResult {
		const UBlueprint* Blueprint = FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		OutInfo.Empty();
		OutInfo.Add(TEXT("name"), Blueprint->GetName());
		OutInfo.Add(TEXT("path"), Blueprint->GetPathName());
		OutInfo.Add(TEXT("parent_class"), Blueprint->ParentClass ? Blueprint->ParentClass->GetName() : TEXT("None"));
		OutInfo.Add(TEXT("blueprint_type"), Blueprint->BlueprintType == BPTYPE_Normal ? TEXT("Normal") : TEXT("Other"));

		if (const USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript) {
			OutInfo.Add(TEXT("num_components"), FString::FromInt(SCS->GetAllNodes().Num()));
		}
		else {
			OutInfo.Add(TEXT("num_components"), TEXT("0"));
		}

		OutInfo.Add(TEXT("num_variables"), FString::FromInt(Blueprint->NewVariables.Num()));

		return FVoidResult::Success();
	}

	auto FBlueprintIntrospectionService::GetBlueprintComponents(
		const FString& BlueprintName,
		TArray<TMap<FString, FString>>& OutComponents
	) -> FVoidResult {
		const UBlueprint* Blueprint = FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		OutComponents.Empty();

		const USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return FVoidResult::Success();
		}

		for (const USCS_Node* Node : SCS->GetAllNodes()) {
			if (!Node || !Node->ComponentTemplate) {
				continue;
			}

			TMap<FString, FString> ComponentInfo;
			ComponentInfo.Add(TEXT("name"), Node->GetVariableName().ToString());
			ComponentInfo.Add(TEXT("type"), Node->ComponentTemplate->GetClass()->GetName());
			ComponentInfo.Add(TEXT("class"), Node->ComponentClass ? Node->ComponentClass->GetName() : TEXT("Unknown"));

			// Get transform from component template if it's a scene component
			if (const USceneComponent* SceneComp = Cast<USceneComponent>(Node->ComponentTemplate)) {
				const FVector Location = SceneComp->GetRelativeLocation();
				ComponentInfo.Add(TEXT("location"),
				                  FString::Printf(TEXT("%.2f,%.2f,%.2f"), Location.X, Location.Y, Location.Z));

				const FRotator Rotation = SceneComp->GetRelativeRotation();
				ComponentInfo.Add(TEXT("rotation"),
				                  FString::Printf(TEXT("%.2f,%.2f,%.2f"), Rotation.Pitch, Rotation.Yaw, Rotation.Roll));

				const FVector Scale = SceneComp->GetRelativeScale3D();
				ComponentInfo.Add(TEXT("scale"), FString::Printf(TEXT("%.2f,%.2f,%.2f"), Scale.X, Scale.Y, Scale.Z));
			}
			else {
				ComponentInfo.Add(TEXT("location"), TEXT("0,0,0"));
				ComponentInfo.Add(TEXT("rotation"), TEXT("0,0,0"));
				ComponentInfo.Add(TEXT("scale"), TEXT("1,1,1"));
			}

			OutComponents.Add(ComponentInfo);
		}

		return FVoidResult::Success();
	}

	auto FBlueprintIntrospectionService::GetBlueprintVariables(
		const FString& BlueprintName) -> TResult<FGetBlueprintVariablesResult> {
		UBlueprint* Blueprint = FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return TResult<FGetBlueprintVariablesResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName)
			);
		}

		FGetBlueprintVariablesResult Result;
		Result.Variables.Reserve(Blueprint->NewVariables.Num());

		for (const FBPVariableDescription& Variable : Blueprint->NewVariables) {
			FBlueprintVariableInfo VariableInfo;
			VariableInfo.Name = Variable.VarName.ToString();
			VariableInfo.Type = Variable.VarType.PinCategory.ToString();
			VariableInfo.Category = Variable.Category.ToString();
			VariableInfo.Tooltip = Variable.FriendlyName.IsEmpty() ? TEXT("") : Variable.FriendlyName;
			VariableInfo.bIsArray = Variable.VarType.IsArray();
			VariableInfo.bIsReference = Variable.VarType.bIsReference;
			VariableInfo.bInstanceEditable = !(Variable.PropertyFlags & CPF_DisableEditOnInstance);
			VariableInfo.bBlueprintReadOnly = (Variable.PropertyFlags & CPF_BlueprintReadOnly) != 0;
			VariableInfo.bExposeOnSpawn = (Variable.PropertyFlags & CPF_ExposeOnSpawn) != 0;

			// Get default value if available
			if (!Variable.DefaultValue.IsEmpty()) {
				VariableInfo.DefaultValue = Variable.DefaultValue;
			}

			Result.Variables.Add(VariableInfo);
		}

		Result.Count = Result.Variables.Num();
		return TResult<FGetBlueprintVariablesResult>::Success(MoveTemp(Result));
	}

	auto FBlueprintIntrospectionService::GetBlueprintPath(const FString& BlueprintName) -> FString {
		const UBlueprint* Blueprint = FindBlueprint(BlueprintName);
		return Blueprint ? Blueprint->GetPathName() : FString();
	}

	auto FBlueprintIntrospectionService::FindBlueprint(const FString& BlueprintName) -> UBlueprint* {
		if (BlueprintName.IsEmpty()) {
			return nullptr;
		}

		// Try direct object path first
		UBlueprint* Blueprint = FindFirstObject<UBlueprint>(*BlueprintName, EFindFirstObjectOptions::NativeFirst);
		if (Blueprint) {
			return Blueprint;
		}

		// If the input looks like a path (contains /), try appending .AssetName format
		if (BlueprintName.Contains(TEXT("/"))) {
			// Extract the asset name from the path (last component after /)
			FString AssetName;
			if (BlueprintName.Split(TEXT("/"), nullptr, &AssetName, ESearchCase::IgnoreCase, ESearchDir::FromEnd)) {
				// Try path with .AssetName suffix
				const FString FullObjectPath = FString::Printf(TEXT("%s.%s"), *BlueprintName, *AssetName);
				Blueprint = FindFirstObject<UBlueprint>(*FullObjectPath, EFindFirstObjectOptions::NativeFirst);
				if (Blueprint) {
					return Blueprint;
				}
			}
		}

		// Try resolving as short name with multiple possible paths
		TArray<FString> PossiblePaths;
		PossiblePaths.Add(FString::Printf(TEXT("/Game/Blueprints/%s.%s"), *BlueprintName, *BlueprintName));
		PossiblePaths.Add(FString::Printf(TEXT("/Game/Tests/Introspection/%s.%s"), *BlueprintName, *BlueprintName));
		PossiblePaths.Add(FString::Printf(TEXT("/Game/Tests/%s.%s"), *BlueprintName, *BlueprintName));
		PossiblePaths.Add(FString::Printf(TEXT("/Game/%s.%s"), *BlueprintName, *BlueprintName));

		for (const FString& Path : PossiblePaths) {
			Blueprint = FindFirstObject<UBlueprint>(*Path, EFindFirstObjectOptions::NativeFirst);
			if (Blueprint) {
				return Blueprint;
			}
		}

		// Search through all loaded blueprints
		for (TObjectIterator<UBlueprint> It; It; ++It) {
			UBlueprint* CurrentBP = *It;

			// Check exact name match
			if (CurrentBP->GetName() == BlueprintName) {
				return CurrentBP;
			}

			// Check if the blueprint path contains the name (for better matching)
			FString BPPath = CurrentBP->GetPathName();
			if (BPPath.Contains(BlueprintName)) {
				// Make sure this is not a partial match by checking the component parts
				TArray<FString> PathParts;
				BPPath.ParseIntoArray(PathParts, TEXT("/"));
				for (const FString& Part : PathParts) {
					if (Part == BlueprintName) {
						return CurrentBP;
					}
				}
			}

			// For transient blueprints, also check if the name matches after removing the /Engine/Transient. prefix
			if (BPPath.StartsWith(TEXT("/Engine/Transient."))) {
				const FString TransientPrefix = TEXT("/Engine/Transient.");
				FString TransientName = BPPath.RightChop(TransientPrefix.Len());
				if (TransientName == BlueprintName) {
					return CurrentBP;
				}
			}
		}

		return nullptr;
	}

	auto FBlueprintIntrospectionService::ResolveBlueprintPath(const FString& BlueprintName) -> FString {
		if (BlueprintName.Contains(TEXT("/"))) {
			return BlueprintName;
		}

		// Default to /Game/Blueprints/ if just a name is provided
		return FString::Printf(TEXT("/Game/Blueprints/%s.%s"), *BlueprintName, *BlueprintName);
	}

	auto FBlueprintIntrospectionService::GetComponentHierarchy(
		const FComponentHierarchyParams& Params) -> TResult<FComponentHierarchyResult> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<FComponentHierarchyResult>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}

		// Find blueprint
		const UBlueprint* Blueprint = FindBlueprint(Params.BlueprintName);
		if (!Blueprint) {
			return TResult<FComponentHierarchyResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *Params.BlueprintName)
			);
		}

		const USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return TResult<FComponentHierarchyResult>::Failure(EErrorCode::BlueprintHasNoConstructionScript, TEXT("Blueprint has no construction script"));
		}

		// Build hierarchy - add ALL nodes to flat array
		FComponentHierarchyResult Result;
		Result.RootCount = SCS->GetRootNodes().Num();
		Result.TotalComponents = SCS->GetAllNodes().Num();

		UE_LOG(LogTemp, Display, TEXT("GetComponentHierarchy - Total nodes: %d, Root nodes: %d"),
			SCS->GetAllNodes().Num(), SCS->GetRootNodes().Num());

		// Add all nodes to hierarchy array (flat structure with parent info in each node)
		for (const USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node) {
				UE_LOG(LogTemp, Display, TEXT("GetComponentHierarchy - Adding node: %s"),
					*Node->GetVariableName().ToString());
				Result.Hierarchy.Add(MakeShared<FJsonValueObject>(BuildHierarchyNode(Node, false)));
			}
		}

		return TResult<FComponentHierarchyResult>::Success(MoveTemp(Result));
	}

	auto FBlueprintIntrospectionService::BuildHierarchyNode(const USCS_Node* Node, bool bIncludeChildren) -> TSharedPtr<FJsonObject> {
		auto NodeObj = MakeShared<FJsonObject>();

		if (!Node || !Node->ComponentTemplate) {
			return NodeObj;
		}

		UE_LOG(LogTemp, Display, TEXT("BuildHierarchyNode - Processing node: %s, Type: %s"),
			*Node->GetVariableName().ToString(),
			Node->ComponentTemplate ? *Node->ComponentTemplate->GetClass()->GetName() : TEXT("None"));

		// Basic node info
		NodeObj->SetStringField(TEXT("name"), Node->GetVariableName().ToString());
		NodeObj->SetStringField(TEXT("type"), Node->ComponentTemplate->GetClass()->GetName());
		NodeObj->SetBoolField(TEXT("is_scene_component"), Node->ComponentTemplate->IsA<USceneComponent>());
		NodeObj->SetBoolField(TEXT("is_root"), Node->ParentComponentOrVariableName.IsNone());

		// Parent info
		if (!Node->ParentComponentOrVariableName.IsNone()) {
			NodeObj->SetStringField(TEXT("parent"), Node->ParentComponentOrVariableName.ToString());
		}

		// Transform info (if scene component)
		if (const USceneComponent* SceneComp = Cast<USceneComponent>(Node->ComponentTemplate)) {
			const auto TransformObj = MakeShared<FJsonObject>();

			FVector Location = SceneComp->GetRelativeLocation();
			auto LocationArray = TArray<TSharedPtr<FJsonValue>>();
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.X));
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Y));
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Z));
			TransformObj->SetArrayField(TEXT("location"), LocationArray);

			FRotator Rotation = SceneComp->GetRelativeRotation();
			auto RotationArray = TArray<TSharedPtr<FJsonValue>>();
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Pitch));
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Yaw));
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Roll));
			TransformObj->SetArrayField(TEXT("rotation"), RotationArray);

			FVector Scale = SceneComp->GetRelativeScale3D();
			auto ScaleArray = TArray<TSharedPtr<FJsonValue>>();
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.X));
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Y));
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Z));
			TransformObj->SetArrayField(TEXT("scale"), ScaleArray);

			NodeObj->SetObjectField(TEXT("transform"), TransformObj);
		}

		// Recursively build children if requested
		if (bIncludeChildren) {
			TArray<TSharedPtr<FJsonValue>> ChildrenArray;
			for (const USCS_Node* ChildNode : Node->GetChildNodes()) {
				if (ChildNode) {
					ChildrenArray.Add(MakeShared<FJsonValueObject>(BuildHierarchyNode(ChildNode, true)));
				}
			}
			NodeObj->SetArrayField(TEXT("children"), ChildrenArray);
			NodeObj->SetNumberField(TEXT("child_count"), ChildrenArray.Num());
		} else {
			NodeObj->SetNumberField(TEXT("child_count"), Node->GetChildNodes().Num());
		}

		return NodeObj;
	}

	auto FBlueprintIntrospectionService::GetComponentProperties(
		const FComponentPropertiesParams& Params) -> TResult<FComponentPropertiesResult> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<FComponentPropertiesResult>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}

		if (Params.ComponentName.IsEmpty()) {
			return TResult<FComponentPropertiesResult>::Failure(EErrorCode::InvalidInput, TEXT("Component name cannot be empty"));
		}

		// Find blueprint
		const UBlueprint* Blueprint = FindBlueprint(Params.BlueprintName);
		if (!Blueprint) {
			return TResult<FComponentPropertiesResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *Params.BlueprintName)
			);
		}

		// Find the component in the blueprint
		const USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return TResult<FComponentPropertiesResult>::Failure(EErrorCode::BlueprintHasNoConstructionScript, TEXT("Blueprint has no construction script"));
		}

		const USCS_Node* TargetNode = nullptr;
		for (const USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == Params.ComponentName) {
				TargetNode = Node;
				break;
			}
		}

		if (!TargetNode || !TargetNode->ComponentTemplate) {
			return TResult<FComponentPropertiesResult>::Failure(
				FString::Printf(TEXT("Component '%s' not found in blueprint"), *Params.ComponentName)
			);
		}

		// Build properties object
		FComponentPropertiesResult Result;
		Result.Properties = MakeShared<FJsonObject>();
		UActorComponent* ComponentTemplate = TargetNode->ComponentTemplate;

		// Basic info
		Result.Properties->SetStringField(TEXT("name"), TargetNode->GetVariableName().ToString());
		Result.Properties->SetStringField(TEXT("type"), ComponentTemplate->GetClass()->GetName());
		Result.Properties->SetStringField(TEXT("class_path"), ComponentTemplate->GetClass()->GetPathName());

		// Transform properties (if SceneComponent)
		if (const USceneComponent* SceneComp = Cast<USceneComponent>(ComponentTemplate)) {
			const auto TransformObj = MakeShared<FJsonObject>();

			FVector Location = SceneComp->GetRelativeLocation();
			auto LocationArray = TArray<TSharedPtr<FJsonValue>>();
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.X));
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Y));
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Z));
			TransformObj->SetArrayField(TEXT("location"), LocationArray);

			FRotator Rotation = SceneComp->GetRelativeRotation();
			auto RotationArray = TArray<TSharedPtr<FJsonValue>>();
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Pitch));
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Yaw));
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Roll));
			TransformObj->SetArrayField(TEXT("rotation"), RotationArray);

			FVector Scale = SceneComp->GetRelativeScale3D();
			auto ScaleArray = TArray<TSharedPtr<FJsonValue>>();
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.X));
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Y));
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Z));
			TransformObj->SetArrayField(TEXT("scale"), ScaleArray);

			Result.Properties->SetObjectField(TEXT("transform"), TransformObj);
			Result.Properties->SetBoolField(TEXT("mobility"), SceneComp->Mobility == EComponentMobility::Movable);
		}

		// Mesh properties (if StaticMeshComponent)
		if (const UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(ComponentTemplate)) {
			if (MeshComp->GetStaticMesh()) {
				Result.Properties->SetStringField(TEXT("static_mesh"), MeshComp->GetStaticMesh()->GetPathName());
			}
			Result.Properties->SetBoolField(TEXT("cast_shadow"), MeshComp->CastShadow);
		}

		// Skeletal mesh properties
		if (const USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(ComponentTemplate)) {
			if (SkelComp->GetSkeletalMeshAsset()) {
				Result.Properties->SetStringField(TEXT("skeletal_mesh"),
				                                  SkelComp->GetSkeletalMeshAsset()->GetPathName());
			}
		}

		// Physics properties (if PrimitiveComponent)
		if (const UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(ComponentTemplate)) {
			const auto PhysicsObj = MakeShared<FJsonObject>();
			PhysicsObj->SetBoolField(TEXT("simulate_physics"), PrimComp->IsSimulatingPhysics());
			PhysicsObj->SetBoolField(TEXT("enable_gravity"), PrimComp->IsGravityEnabled());
			PhysicsObj->SetNumberField(TEXT("mass"), PrimComp->GetMass());
			PhysicsObj->SetNumberField(TEXT("linear_damping"), PrimComp->GetLinearDamping());
			PhysicsObj->SetNumberField(TEXT("angular_damping"), PrimComp->GetAngularDamping());
			PhysicsObj->SetStringField(TEXT("collision_profile"), PrimComp->GetCollisionProfileName().ToString());
			Result.Properties->SetObjectField(TEXT("physics"), PhysicsObj);
		}

		// Light properties (if LightComponent)
		if (const ULightComponent* LightComp = Cast<ULightComponent>(ComponentTemplate)) {
			const auto LightObj = MakeShared<FJsonObject>();
			LightObj->SetNumberField(TEXT("intensity"), LightComp->Intensity);

			FLinearColor Color = LightComp->GetLightColor();
			auto ColorArray = TArray<TSharedPtr<FJsonValue>>();
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.R));
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.G));
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.B));
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.A));
			LightObj->SetArrayField(TEXT("color"), ColorArray);

			LightObj->SetBoolField(TEXT("cast_shadows"), LightComp->CastShadows);
			Result.Properties->SetObjectField(TEXT("light"), LightObj);
		}

		// Movement properties (if CharacterMovementComponent)
		if (const UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(ComponentTemplate)) {
			const auto MovementObj = MakeShared<FJsonObject>();
			MovementObj->SetNumberField(TEXT("max_walk_speed"), MovementComp->MaxWalkSpeed);
			MovementObj->SetNumberField(TEXT("max_acceleration"), MovementComp->MaxAcceleration);
			MovementObj->SetNumberField(TEXT("jump_z_velocity"), MovementComp->JumpZVelocity);
			MovementObj->SetNumberField(TEXT("gravity_scale"), MovementComp->GravityScale);
			Result.Properties->SetObjectField(TEXT("movement"), MovementObj);
		}

		return TResult<FComponentPropertiesResult>::Success(MoveTemp(Result));
	}

	auto FBlueprintIntrospectionService::RemoveComponent(
		const FRemoveComponentParams& Params) -> TResult<FRemoveComponentResult> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<FRemoveComponentResult>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}

		if (Params.ComponentName.IsEmpty()) {
			return TResult<FRemoveComponentResult>::Failure(EErrorCode::InvalidInput, TEXT("Component name cannot be empty"));
		}

		// Find blueprint
		UBlueprint* Blueprint = FindBlueprint(Params.BlueprintName);
		if (!Blueprint) {
			return TResult<FRemoveComponentResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *Params.BlueprintName)
			);
		}

		// Find the component in the blueprint
		USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return TResult<FRemoveComponentResult>::Failure(EErrorCode::BlueprintHasNoConstructionScript, TEXT("Blueprint has no construction script"));
		}

		USCS_Node* NodeToRemove = nullptr;
		for (USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == Params.ComponentName) {
				NodeToRemove = Node;
				break;
			}
		}

		if (!NodeToRemove) {
			return TResult<FRemoveComponentResult>::Failure(
				FString::Printf(TEXT("Component '%s' not found in blueprint"), *Params.ComponentName)
			);
		}

		// Remove the node
		SCS->RemoveNode(NodeToRemove);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		// Create result
		FRemoveComponentResult Result;
		Result.BlueprintName = Params.BlueprintName;
		Result.ComponentName = Params.ComponentName;
		Result.Message = FString::Printf(
			TEXT("Component '%s' removed from blueprint '%s'"),
			*Params.ComponentName,
			*Params.BlueprintName
		);

		return TResult<FRemoveComponentResult>::Success(MoveTemp(Result));
	}

	auto FBlueprintIntrospectionService::RenameComponent(
		const FRenameComponentParams& Params) -> TResult<FRenameComponentResult> {
		// Validate input
		if (Params.NewName.IsEmpty()) {
			return TResult<FRenameComponentResult>::Failure(EErrorCode::InvalidInput, TEXT("New component name cannot be empty"));
		}

		// Find the blueprint
		UBlueprint* Blueprint = FindBlueprint(Params.BlueprintName);
		if (!Blueprint) {
			return TResult<FRenameComponentResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *Params.BlueprintName)
			);
		}

		// Validate blueprint has construction script
		const USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return TResult<FRenameComponentResult>::Failure(EErrorCode::BlueprintHasNoConstructionScript, TEXT("Blueprint has no construction script"));
		}

		// Find the component to rename
		USCS_Node* TargetNode = nullptr;
		for (USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == Params.OldName) {
				TargetNode = Node;
				break;
			}
		}

		if (!TargetNode) {
			return TResult<FRenameComponentResult>::Failure(
				FString::Printf(TEXT("Component '%s' not found in blueprint"), *Params.OldName)
			);
		}

		// Check if new name already exists
		for (const USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == Params.NewName) {
				return TResult<FRenameComponentResult>::Failure(
					FString::Printf(TEXT("Component with name '%s' already exists"), *Params.NewName)
				);
			}
		}

		// Rename the component
		const auto NewFName = FName(*Params.NewName);
		FBlueprintEditorUtils::RenameComponentMemberVariable(Blueprint, TargetNode, NewFName);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		// Create result
		FRenameComponentResult Result;
		Result.BlueprintName = Params.BlueprintName;
		Result.OldName = Params.OldName;
		Result.NewName = Params.NewName;
		Result.Message = FString::Printf(
			TEXT("Component renamed from '%s' to '%s' in blueprint '%s'"),
			*Params.OldName,
			*Params.NewName,
			*Params.BlueprintName
		);

		return TResult<FRenameComponentResult>::Success(MoveTemp(Result));
	}

}
