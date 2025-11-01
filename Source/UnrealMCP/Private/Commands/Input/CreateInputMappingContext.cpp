#include "Commands/Input/CreateInputMappingContext.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/InputService.h"

namespace UnrealMCP {

	auto FCreateInputMappingContext::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		TResult<FInputMappingContextParams> ParamsResult =
			FInputMappingContextParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const TResult<UInputMappingContext*> Result =
			FInputService::CreateInputMappingContext(ParamsResult.GetValue());

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		const auto& [Name, Path] = ParamsResult.GetValue();

		const FString AssetName = Name.StartsWith(TEXT("IMC_")) ? Name : FString::Printf(TEXT("IMC_%s"), *Name);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("name"), Name);
			Data->SetStringField(TEXT("asset_path"), Path / AssetName);
		});
	}
}
