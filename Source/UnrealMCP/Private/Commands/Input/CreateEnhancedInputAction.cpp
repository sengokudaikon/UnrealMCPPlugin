#include "Commands/Input/CreateEnhancedInputAction.h"
#include "Commands/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

auto FCreateEnhancedInputAction::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FInputActionParams> ParamsResult =
		UnrealMCP::FInputActionParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::TResult<UInputAction*> Result =
		UnrealMCP::FInputService::CreateInputAction(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const UnrealMCP::FInputActionParams& ParsedParams = ParamsResult.GetValue();
	const UInputAction* InputAction = Result.GetValue();

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("name"), ParsedParams.Name);
	Response->SetStringField(TEXT("value_type"), ParsedParams.ValueType);
	Response->SetStringField(TEXT("asset_path"), ParsedParams.Path / FString::Printf(TEXT("IA_%s"), *ParsedParams.Name));
	return Response;
}
