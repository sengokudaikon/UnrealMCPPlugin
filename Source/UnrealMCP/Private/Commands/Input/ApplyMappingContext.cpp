#include "Commands/Input/ApplyMappingContext.h"
#include "Core/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

auto FApplyMappingContext::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FApplyMappingContextParams> ParamsResult =
		UnrealMCP::FApplyMappingContextParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FInputService::ApplyMappingContext(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const UnrealMCP::FApplyMappingContextParams& ParsedParams = ParamsResult.GetValue();

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("context_path"), ParsedParams.ContextPath);
		Data->SetNumberField(TEXT("priority"), ParsedParams.Priority);
	});
}
