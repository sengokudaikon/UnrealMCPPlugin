#include "Commands/Blueprint/SetPawnProperties.h"

#include "Commands/CommonUtils.h"
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

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("blueprint"), BlueprintName);
	Response->SetBoolField(TEXT("success"), true);
	return Response;
}
