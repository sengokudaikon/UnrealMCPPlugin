#include "Types/InputTypes.h"
#include "Core/ErrorTypes.h"

namespace UnrealMCP {
	auto FInputActionParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FInputActionParams> {
		if (!Json.IsValid()) {
			return TResult<FInputActionParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FInputActionParams Params;

		if (!Json->TryGetStringField(TEXT("name"), Params.Name)) {
			return TResult<FInputActionParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'name' parameter"));
		}

		Json->TryGetStringField(TEXT("value_type"), Params.ValueType);

		FString Path;
		if (Json->TryGetStringField(TEXT("path"), Path)) {
			Params.Path = Path;
		}

		return TResult<FInputActionParams>::Success(MoveTemp(Params));
	}

	auto FInputMappingContextParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FInputMappingContextParams> {
		if (!Json.IsValid()) {
			return TResult<FInputMappingContextParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FInputMappingContextParams Params;

		if (!Json->TryGetStringField(TEXT("name"), Params.Name)) {
			return TResult<FInputMappingContextParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'name' parameter"));
		}

		FString Path;
		if (Json->TryGetStringField(TEXT("path"), Path)) {
			Params.Path = Path;
		}

		return TResult<FInputMappingContextParams>::Success(MoveTemp(Params));
	}

	auto FAddMappingParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FAddMappingParams> {
		if (!Json.IsValid()) {
			return TResult<FAddMappingParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FAddMappingParams Params;

		if (!Json->TryGetStringField(TEXT("context_path"), Params.ContextPath)) {
			return TResult<FAddMappingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'context_path' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("action_path"), Params.ActionPath)) {
			return TResult<FAddMappingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'action_path' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("key"), Params.Key)) {
			return TResult<FAddMappingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'key' parameter"));
		}

		return TResult<FAddMappingParams>::Success(MoveTemp(Params));
	}

	auto FApplyMappingContextParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FApplyMappingContextParams> {
		if (!Json.IsValid()) {
			return TResult<FApplyMappingContextParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FApplyMappingContextParams Params;

		if (!Json->TryGetStringField(TEXT("context_path"), Params.ContextPath)) {
			return TResult<FApplyMappingContextParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'context_path' parameter"));
		}

		if (Json->HasField(TEXT("priority"))) {
			Params.Priority = static_cast<int32>(Json->GetNumberField(TEXT("priority")));
		}

		return TResult<FApplyMappingContextParams>::Success(MoveTemp(Params));
	}

	auto FRemoveMappingContextParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FRemoveMappingContextParams> {
		if (!Json.IsValid()) {
			return TResult<FRemoveMappingContextParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FRemoveMappingContextParams Params;

		if (!Json->TryGetStringField(TEXT("context_path"), Params.ContextPath)) {
			return TResult<FRemoveMappingContextParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'context_path' parameter"));
		}

		return TResult<FRemoveMappingContextParams>::Success(MoveTemp(Params));
	}

	auto FLegacyInputMappingParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FLegacyInputMappingParams> {
		if (!Json.IsValid()) {
			return TResult<FLegacyInputMappingParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FLegacyInputMappingParams Params;

		if (!Json->TryGetStringField(TEXT("action_name"), Params.ActionName)) {
			return TResult<FLegacyInputMappingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'action_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("key"), Params.Key)) {
			return TResult<FLegacyInputMappingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'key' parameter"));
		}

		// Optional modifier fields
		if (Json->HasField(TEXT("shift"))) {
			Params.bShift = Json->GetBoolField(TEXT("shift"));
		}
		if (Json->HasField(TEXT("ctrl"))) {
			Params.bCtrl = Json->GetBoolField(TEXT("ctrl"));
		}
		if (Json->HasField(TEXT("alt"))) {
			Params.bAlt = Json->GetBoolField(TEXT("alt"));
		}
		if (Json->HasField(TEXT("cmd"))) {
			Params.bCmd = Json->GetBoolField(TEXT("cmd"));
		}

		return TResult<FLegacyInputMappingParams>::Success(MoveTemp(Params));
	}

	auto FCreateInputActionResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("name"), Name);
		Result->SetStringField(TEXT("value_type"), ValueType);
		Result->SetStringField(TEXT("asset_path"), AssetPath);
		return Result;
	}
}
