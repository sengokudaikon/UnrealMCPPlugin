#include "Commands/Input/RemoveEnhancedInputMapping.h"
#include "Commands/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

auto FRemoveEnhancedInputMapping::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FAddMappingParams> ParamsResult =
		UnrealMCP::FAddMappingParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FInputService::RemoveMappingFromContext(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const UnrealMCP::FAddMappingParams& ParsedParams = ParamsResult.GetValue();

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("context_path"), ParsedParams.ContextPath);
	Response->SetStringField(TEXT("action_path"), ParsedParams.ActionPath);
	return Response;
}
