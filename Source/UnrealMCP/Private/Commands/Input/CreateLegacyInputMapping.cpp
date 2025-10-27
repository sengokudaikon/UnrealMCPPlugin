#include "Commands/Input/CreateLegacyInputMapping.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/InputService.h"

namespace UnrealMCP {

	auto FCreateLegacyInputMapping::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		TResult<FLegacyInputMappingParams> ParamsResult =
			FLegacyInputMappingParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const FVoidResult Result =
			FInputService::CreateLegacyInputMapping(ParamsResult.GetValue());

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		const auto& [ActionName, Key, bShift, bCtrl, bAlt, bCmd] = ParamsResult.GetValue();

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("action_name"), ActionName);
			Data->SetStringField(TEXT("key"), Key);
			Data->SetBoolField(TEXT("shift"), bShift);
			Data->SetBoolField(TEXT("ctrl"), bCtrl);
			Data->SetBoolField(TEXT("alt"), bAlt);
			Data->SetBoolField(TEXT("cmd"), bCmd);
		});
	}
}
