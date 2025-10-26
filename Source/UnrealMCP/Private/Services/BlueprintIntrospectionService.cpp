#include "Services/BlueprintIntrospectionService.h"
#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Components/SceneComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/UObjectIterator.h"

namespace UnrealMCP {

	auto FBlueprintIntrospectionService::ListBlueprints(
		const FString& Path,
		const bool bRecursive,
		TArray<FString>& OutBlueprints
	) -> FVoidResult {
		OutBlueprints.Empty();

		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
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

	bool FBlueprintIntrospectionService::BlueprintExists(const FString& BlueprintName) {
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
		} else {
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
				ComponentInfo.Add(TEXT("location"), FString::Printf(TEXT("%.2f,%.2f,%.2f"), Location.X, Location.Y, Location.Z));

				const FRotator Rotation = SceneComp->GetRelativeRotation();
				ComponentInfo.Add(TEXT("rotation"), FString::Printf(TEXT("%.2f,%.2f,%.2f"), Rotation.Pitch, Rotation.Yaw, Rotation.Roll));

				const FVector Scale = SceneComp->GetRelativeScale3D();
				ComponentInfo.Add(TEXT("scale"), FString::Printf(TEXT("%.2f,%.2f,%.2f"), Scale.X, Scale.Y, Scale.Z));
			} else {
				ComponentInfo.Add(TEXT("location"), TEXT("0,0,0"));
				ComponentInfo.Add(TEXT("rotation"), TEXT("0,0,0"));
				ComponentInfo.Add(TEXT("scale"), TEXT("1,1,1"));
			}

			OutComponents.Add(ComponentInfo);
		}

		return FVoidResult::Success();
	}

	auto FBlueprintIntrospectionService::GetBlueprintVariables(
		const FString& BlueprintName,
		TArray<TMap<FString, FString>>& OutVariables
	) -> FVoidResult {
		UBlueprint* Blueprint = FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		OutVariables.Empty();

		for (const FBPVariableDescription& Variable : Blueprint->NewVariables) {
			TMap<FString, FString> VarInfo;
			VarInfo.Add(TEXT("name"), Variable.VarName.ToString());
			VarInfo.Add(TEXT("type"), Variable.VarType.PinCategory.ToString());
			VarInfo.Add(TEXT("category"), Variable.Category.ToString());
			VarInfo.Add(TEXT("tooltip"), Variable.FriendlyName.IsEmpty() ? TEXT("") : Variable.FriendlyName);

			OutVariables.Add(VarInfo);
		}

		return FVoidResult::Success();
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

		// Try resolving as short name
		const FString FullPath = ResolveBlueprintPath(BlueprintName);
		if (!FullPath.IsEmpty()) {
			Blueprint = FindFirstObject<UBlueprint>(*FullPath, EFindFirstObjectOptions::NativeFirst);
			if (Blueprint) {
				return Blueprint;
			}
		}

		// Search through all loaded blueprints
		for (TObjectIterator<UBlueprint> It; It; ++It) {
			UBlueprint* CurrentBP = *It;
			if (CurrentBP->GetName() == BlueprintName) {
				return CurrentBP;
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

	auto FBlueprintIntrospectionService::GetComponentHierarchy(const FComponentHierarchyParams& Params) -> TResult<FComponentHierarchyResult> {
		// Validate input parameters
		if (Params.BlueprintName.IsEmpty()) {
			return TResult<FComponentHierarchyResult>::Failure(TEXT("Blueprint name cannot be empty"));
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
			return TResult<FComponentHierarchyResult>::Failure(TEXT("Blueprint has no construction script"));
		}

		// Build hierarchy starting from root nodes
		FComponentHierarchyResult Result;
		Result.RootCount = 0;
		Result.TotalComponents = SCS->GetAllNodes().Num();

		for (const USCS_Node* RootNode : SCS->GetRootNodes()) {
			if (RootNode) {
				Result.Hierarchy.Add(MakeShared<FJsonValueObject>(BuildHierarchyNode(RootNode)));
				Result.RootCount++;
			}
		}

		return TResult<FComponentHierarchyResult>::Success(MoveTemp(Result));
	}

	TSharedPtr<FJsonObject> FBlueprintIntrospectionService::BuildHierarchyNode(const USCS_Node* Node) {
		auto NodeObj = MakeShared<FJsonObject>();

		if (!Node || !Node->ComponentTemplate) {
			return NodeObj;
		}

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

		// Recursively build children
		TArray<TSharedPtr<FJsonValue>> ChildrenArray;
		for (const USCS_Node* ChildNode : Node->GetChildNodes()) {
			if (ChildNode) {
				ChildrenArray.Add(MakeShared<FJsonValueObject>(BuildHierarchyNode(ChildNode)));
			}
		}

		NodeObj->SetArrayField(TEXT("children"), ChildrenArray);
		NodeObj->SetNumberField(TEXT("child_count"), ChildrenArray.Num());

		return NodeObj;
	}

} // namespace UnrealMCP
