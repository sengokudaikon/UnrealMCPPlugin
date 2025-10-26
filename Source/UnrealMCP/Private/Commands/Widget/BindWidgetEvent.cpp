#include "Commands/UMG/BindWidgetEvent.h"
#include "Core/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"

auto FBindWidgetEvent::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FWidgetEventBindingParams> ParamsResult =
		UnrealMCP::FWidgetEventBindingParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FWidgetService::BindWidgetEvent(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const UnrealMCP::FWidgetEventBindingParams& ParsedParams = ParamsResult.GetValue();
	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("widget_component_name"), ParsedParams.WidgetComponentName);
		Data->SetStringField(TEXT("event_name"), ParsedParams.EventName);
	});
}