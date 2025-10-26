#include "Commands/Blueprint/SetPawnProperties.h"

#include "Core/CommonUtils.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

auto FSetPawnProperties::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
	}

	if (!Params.IsValid() || Params->Values.Num() == 0) {
		return FCommonUtils::CreateErrorResponse(TEXT("No properties specified to set"));
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FBlueprintService::SetPawnProperties(BlueprintName, Params);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("blueprint"), BlueprintName);
	});
}
