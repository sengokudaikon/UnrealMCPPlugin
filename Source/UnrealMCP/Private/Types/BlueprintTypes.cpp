#include "Types/BlueprintTypes.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"

namespace UnrealMCP {
	auto FBlueprintSpawnParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FBlueprintSpawnParams> {
		if (!Json.IsValid()) {
			return TResult<FBlueprintSpawnParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FBlueprintSpawnParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FBlueprintSpawnParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("actor_name"), Params.ActorName)) {
			return TResult<FBlueprintSpawnParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'actor_name' parameter"));
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

		return TResult<FBlueprintSpawnParams>::Success(MoveTemp(Params));
	}

	auto FBlueprintCreationParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FBlueprintCreationParams> {
		if (!Json.IsValid()) {
			return TResult<FBlueprintCreationParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FBlueprintCreationParams Params;

		if (!Json->TryGetStringField(TEXT("name"), Params.Name)) {
			return TResult<FBlueprintCreationParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'name' parameter"));
		}

		Json->TryGetStringField(TEXT("parent_class"), Params.ParentClass);

		FString PackagePath;
		if (Json->TryGetStringField(TEXT("package_path"), PackagePath)) {
			Params.PackagePath = PackagePath;
		}

		return TResult<FBlueprintCreationParams>::Success(MoveTemp(Params));
	}

	auto FDeleteBlueprintParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FDeleteBlueprintParams> {
		if (!Json.IsValid()) {
			return TResult<FDeleteBlueprintParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FDeleteBlueprintParams Params;

		if (!Json->TryGetStringField(TEXT("blueprint_name"), Params.BlueprintName)) {
			return TResult<FDeleteBlueprintParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter"));
		}

		return TResult<FDeleteBlueprintParams>::Success(MoveTemp(Params));
	}

	auto FDeleteBlueprintResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("deleted_path"), DeletedPath);
		return Result;
	}
}
