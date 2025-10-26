#include "Commands/Blueprint/SetComponentProperty.h"

#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

namespace UnrealMCP {

auto FSetComponentProperty::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
	}

	FString ComponentName;
	if (!Params->TryGetStringField(TEXT("component_name"), ComponentName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'component_name' parameter"));
	}

	TResult<FPropertyParams> ParamsResult =
		FPropertyParams::FromJson(Params, TEXT("component_name"));

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const FVoidResult Result =
		FBlueprintService::SetComponentProperty(
			BlueprintName,
			ComponentName,
			ParamsResult.GetValue()
		);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("component"), ComponentName);
		Data->SetStringField(TEXT("property"), ParamsResult.GetValue().PropertyName);
	});
}

}
