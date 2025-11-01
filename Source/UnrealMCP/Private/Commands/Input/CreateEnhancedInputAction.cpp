#include "Commands/Input/CreateEnhancedInputAction.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/InputService.h"

namespace UnrealMCP {

	auto FCreateEnhancedInputAction::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		TResult<FInputActionParams> ParamsResult = FInputActionParams::FromJson(Params);
		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const TResult<UInputAction*> ServiceResult = FInputService::CreateInputAction(ParamsResult.GetValue());
		if (ServiceResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ServiceResult.GetError());
		}

		const auto& [Name, ValueType, Path] = ParamsResult.GetValue();

		FCreateInputActionResult Result;
		Result.Name = Name;
		Result.ValueType = ValueType;
		const FString AssetName = Name.StartsWith(TEXT("IA_")) ? Name : FString::Printf(TEXT("IA_%s"), *Name);
		Result.AssetPath = Path / AssetName;

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetObjectField(TEXT("result"), Result.ToJson());
		});
	}
}
