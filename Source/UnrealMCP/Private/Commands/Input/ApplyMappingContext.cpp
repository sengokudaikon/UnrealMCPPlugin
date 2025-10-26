#include "Commands/Input/ApplyMappingContext.h"
#include "Core/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

namespace UnrealMCP {

auto FApplyMappingContext::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	TResult<FApplyMappingContextParams> ParamsResult =
		FApplyMappingContextParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const FVoidResult Result =
		FInputService::ApplyMappingContext(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const auto& [ContextPath, Priority] = ParamsResult.GetValue();

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("context_path"), ContextPath);
		Data->SetNumberField(TEXT("priority"), Priority);
	});
}}
