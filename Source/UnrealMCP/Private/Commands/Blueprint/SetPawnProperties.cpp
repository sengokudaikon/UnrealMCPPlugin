#include "Commands/Blueprint/SetPawnProperties.h"

#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

namespace UnrealMCP {

	auto FSetPawnProperties::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		FString BlueprintName;
		if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter")));
		}

		if (!Params.IsValid() || Params->Values.Num() == 0) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("No properties specified to set")));
		}

		const FVoidResult Result =
			FBlueprintService::SetPawnProperties(BlueprintName, Params);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("blueprint"), BlueprintName);
		});
	}

}
