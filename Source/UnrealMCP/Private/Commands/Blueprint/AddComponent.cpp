#include "Commands/Blueprint/AddComponent.h"

#include "Commands/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

auto FAddComponent::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FComponentParams> ParamsResult =
		UnrealMCP::FComponentParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::TResult<UBlueprint*> Result =
		UnrealMCP::FBlueprintService::AddComponent(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const UnrealMCP::FComponentParams& ComponentParams = ParamsResult.GetValue();
	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("blueprint_name"), ComponentParams.BlueprintName);
	Response->SetStringField(TEXT("component_name"), ComponentParams.ComponentName);
	Response->SetStringField(TEXT("component_type"), ComponentParams.ComponentType);
	Response->SetBoolField(TEXT("success"), true);
	return Response;
}
