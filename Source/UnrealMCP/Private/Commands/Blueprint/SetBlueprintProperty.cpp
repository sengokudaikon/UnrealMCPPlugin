#include "Commands/Blueprint/SetBlueprintProperty.h"

#include "Commands/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

auto FSetBlueprintProperty::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
	// Extract blueprint name
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
	}

	UnrealMCP::TResult<UnrealMCP::FPropertyParams> ParamsResult =
		UnrealMCP::FPropertyParams::FromJson(Params, TEXT("blueprint_name"));

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FBlueprintService::SetBlueprintProperty(
			BlueprintName,
			ParamsResult.GetValue()
		);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("property"), ParamsResult.GetValue().PropertyName);
	Response->SetBoolField(TEXT("success"), true);
	return Response;
}
