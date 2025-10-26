#include "Commands/Widget/BindWidgetEvent.h"
#include "Core/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"

namespace UnrealMCP {

auto FBindWidgetEvent::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	TResult<FWidgetEventBindingParams> ParamsResult =
		FWidgetEventBindingParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const FVoidResult Result =
		FWidgetService::BindWidgetEvent(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const FWidgetEventBindingParams& ParsedParams = ParamsResult.GetValue();
	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("widget_component_name"), ParsedParams.WidgetComponentName);
		Data->SetStringField(TEXT("event_name"), ParsedParams.EventName);
	});
}}