#include "Commands/Input/RemoveMappingContext.h"
#include "Core/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

namespace UnrealMCP {

auto FRemoveMappingContext::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	TResult<FRemoveMappingContextParams> ParamsResult =
		FRemoveMappingContextParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const FVoidResult Result =
		FInputService::RemoveMappingContext(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const auto& [ContextPath] = ParamsResult.GetValue();

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("context_path"), ContextPath);
	});
}}
