#include "Commands/Input/AddEnhancedInputMapping.h"
#include "Core/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

auto FAddEnhancedInputMapping::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FAddMappingParams> ParamsResult =
		UnrealMCP::FAddMappingParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FInputService::AddMappingToContext(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const UnrealMCP::FAddMappingParams& ParsedParams = ParamsResult.GetValue();

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("context_path"), ParsedParams.ContextPath);
		Data->SetStringField(TEXT("action_path"), ParsedParams.ActionPath);
		Data->SetStringField(TEXT("key"), ParsedParams.Key);
	});
}
