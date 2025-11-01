#include "Types/ComponentTypes.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"

namespace UnrealMCP {
	auto FComponentParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentParams> {
		if (!Json.IsValid()) {
			return TResult<FComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FComponentParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("component_type"), Params.ComponentType)) {
			return TResult<FComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'component_type' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("component_name"), Params.ComponentName)) {
			return TResult<FComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'component_name' parameter"));
		}

		FString MeshType;
		if (Json->TryGetStringField(TEXT("static_mesh"), MeshType)) {
			Params.MeshType = MeshType;
		}

		if (Json->HasField(TEXT("location"))) {
			Params.Location = FCommonUtils::GetVectorFromJson(Json, TEXT("location"));
		}

		if (Json->HasField(TEXT("rotation"))) {
			Params.Rotation = FCommonUtils::GetRotatorFromJson(Json, TEXT("rotation"));
		}

		if (Json->HasField(TEXT("scale"))) {
			Params.Scale = FCommonUtils::GetVectorFromJson(Json, TEXT("scale"));
		}

		if (Json->HasField(TEXT("component_properties"))) {
			Params.Properties = Json->GetObjectField(TEXT("component_properties"));
		}

		return TResult<FComponentParams>::Success(MoveTemp(Params));
	}

	auto FPropertyParams::FromJson(const TSharedPtr<FJsonObject>& Json,
	                               const FString& TargetFieldName) -> TResult<FPropertyParams> {
		if (!Json.IsValid()) {
			return TResult<FPropertyParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FPropertyParams Params;

		if (!Json->TryGetStringField(*TargetFieldName, Params.TargetName)) {
			return TResult<FPropertyParams>::Failure(EErrorCode::InvalidInput, FString::Printf(TEXT("Missing '%s' parameter"), *TargetFieldName));
		}

		if (!Json->TryGetStringField(TEXT("property_name"), Params.PropertyName)) {
			return TResult<FPropertyParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'property_name' parameter"));
		}

		if (!Json->HasField(TEXT("property_value"))) {
			return TResult<FPropertyParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'property_value' parameter"));
		}

		Params.PropertyValue = Json->Values.FindRef(TEXT("property_value"));

		return TResult<FPropertyParams>::Success(MoveTemp(Params));
	}

	auto FPhysicsParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FPhysicsParams> {
		if (!Json.IsValid()) {
			return TResult<FPhysicsParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FPhysicsParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FPhysicsParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("component_name"), Params.ComponentName)) {
			return TResult<FPhysicsParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'component_name' parameter"));
		}

		if (Json->HasField(TEXT("simulate_physics"))) {
			Params.bSimulatePhysics = Json->GetBoolField(TEXT("simulate_physics"));
		}

		if (Json->HasField(TEXT("mass"))) {
			Params.Mass = Json->GetNumberField(TEXT("mass"));
		}

		if (Json->HasField(TEXT("linear_damping"))) {
			Params.LinearDamping = Json->GetNumberField(TEXT("linear_damping"));
		}

		if (Json->HasField(TEXT("angular_damping"))) {
			Params.AngularDamping = Json->GetNumberField(TEXT("angular_damping"));
		}

		if (Json->HasField(TEXT("gravity_enabled"))) {
			Params.bEnableGravity = Json->GetBoolField(TEXT("gravity_enabled"));
		}

		return TResult<FPhysicsParams>::Success(MoveTemp(Params));
	}

	auto FStaticMeshParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FStaticMeshParams> {
		if (!Json.IsValid()) {
			return TResult<FStaticMeshParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FStaticMeshParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FStaticMeshParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("component_name"), Params.ComponentName)) {
			return TResult<FStaticMeshParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'component_name' parameter"));
		}

		FString StaticMesh;
		if (Json->TryGetStringField(TEXT("static_mesh"), StaticMesh)) {
			Params.StaticMesh = StaticMesh;
		}

		FString Material;
		if (Json->TryGetStringField(TEXT("material"), Material)) {
			Params.Material = Material;
		}

		return TResult<FStaticMeshParams>::Success(MoveTemp(Params));
	}

	auto FComponentTransformParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentTransformParams> {
		if (!Json.IsValid()) {
			return TResult<FComponentTransformParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FComponentTransformParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FComponentTransformParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("component_name"), Params.ComponentName)) {
			return TResult<FComponentTransformParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'component_name' parameter"));
		}

		// Optional transform fields
		if (Json->HasField(TEXT("location"))) {
			Params.Location = FCommonUtils::GetVectorFromJson(Json, TEXT("location"));
		}

		if (Json->HasField(TEXT("rotation"))) {
			Params.Rotation = FCommonUtils::GetRotatorFromJson(Json, TEXT("rotation"));
		}

		if (Json->HasField(TEXT("scale"))) {
			Params.Scale = FCommonUtils::GetVectorFromJson(Json, TEXT("scale"));
		}

		// Validate that at least one transform property is provided
		if (!Params.Location.IsSet() && !Params.Rotation.IsSet() && !Params.Scale.IsSet()) {
			return TResult<FComponentTransformParams>::Failure(
				EErrorCode::InvalidInput, TEXT("At least one transform property must be provided (location, rotation, or scale)"));
		}

		return TResult<FComponentTransformParams>::Success(MoveTemp(Params));
	}

	auto FComponentTransformResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();

		// Location
		auto LocationArray = TArray<TSharedPtr<FJsonValue>>();
		LocationArray.Add(MakeShared<FJsonValueNumber>(Location.X));
		LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Y));
		LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Z));
		Result->SetArrayField(TEXT("location"), LocationArray);

		// Rotation
		auto RotationArray = TArray<TSharedPtr<FJsonValue>>();
		RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Pitch));
		RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Yaw));
		RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Roll));
		Result->SetArrayField(TEXT("rotation"), RotationArray);

		// Scale
		auto ScaleArray = TArray<TSharedPtr<FJsonValue>>();
		ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.X));
		ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Y));
		ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Z));
		Result->SetArrayField(TEXT("scale"), ScaleArray);

		return Result;
	}

	auto FComponentHierarchyParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentHierarchyParams> {
		if (!Json.IsValid()) {
			return TResult<FComponentHierarchyParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FComponentHierarchyParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FComponentHierarchyParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		return TResult<FComponentHierarchyParams>::Success(MoveTemp(Params));
	}

	auto FComponentHierarchyResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		Result->SetArrayField(TEXT("hierarchy"), Hierarchy);
		Result->SetNumberField(TEXT("root_count"), RootCount);
		Result->SetNumberField(TEXT("total_components"), TotalComponents);
		return Result;
	}

	auto FComponentPropertiesParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentPropertiesParams> {
		if (!Json.IsValid()) {
			return TResult<FComponentPropertiesParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FComponentPropertiesParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FComponentPropertiesParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("component_name"), Params.ComponentName)) {
			return TResult<FComponentPropertiesParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'component_name' parameter"));
		}

		return TResult<FComponentPropertiesParams>::Success(MoveTemp(Params));
	}

	auto FComponentPropertiesResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		if (Properties.IsValid()) {
			Result->SetObjectField(TEXT("properties"), Properties);
		}
		return Result;
	}

	auto FRemoveComponentParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FRemoveComponentParams> {
		if (!Json.IsValid()) {
			return TResult<FRemoveComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FRemoveComponentParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FRemoveComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("component_name"), Params.ComponentName)) {
			return TResult<FRemoveComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'component_name' parameter"));
		}

		return TResult<FRemoveComponentParams>::Success(MoveTemp(Params));
	}

	auto FRemoveComponentResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("blueprint_name"), BlueprintName);
		Result->SetStringField(TEXT("component_name"), ComponentName);
		Result->SetStringField(TEXT("message"), Message);
		return Result;
	}

	auto FRenameComponentParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FRenameComponentParams> {
		if (!Json.IsValid()) {
			return TResult<FRenameComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FRenameComponentParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FRenameComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("old_name"), Params.OldName)) {
			return TResult<FRenameComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'old_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("new_name"), Params.NewName)) {
			return TResult<FRenameComponentParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'new_name' parameter"));
		}

		return TResult<FRenameComponentParams>::Success(MoveTemp(Params));
	}

	auto FRenameComponentResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("blueprint_name"), BlueprintName);
		Result->SetStringField(TEXT("old_name"), OldName);
		Result->SetStringField(TEXT("new_name"), NewName);
		Result->SetStringField(TEXT("message"), Message);
		return Result;
	}
}
