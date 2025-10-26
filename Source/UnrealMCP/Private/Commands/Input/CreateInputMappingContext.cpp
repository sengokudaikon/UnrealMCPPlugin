#include "Commands/Input/CreateInputMappingContext.h"
#include "Core/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

auto FCreateInputMappingContext::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FInputMappingContextParams> ParamsResult =
		UnrealMCP::FInputMappingContextParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::TResult<UInputMappingContext*> Result =
		UnrealMCP::FInputService::CreateInputMappingContext(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const UnrealMCP::FInputMappingContextParams& ParsedParams = ParamsResult.GetValue();

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("name"), ParsedParams.Name);
		Data->SetStringField(TEXT("asset_path"), ParsedParams.Path / FString::Printf(TEXT("IMC_%s"), *ParsedParams.Name));
	});
}
