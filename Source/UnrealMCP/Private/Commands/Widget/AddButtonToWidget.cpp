#include "Commands/UMG/AddButtonToWidget.h"
#include "Commands/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"
#include "Components/Button.h"

auto FAddButtonToWidget::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FButtonParams> ParamsResult =
		UnrealMCP::FButtonParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::TResult<UButton*> Result =
		UnrealMCP::FWidgetService::AddButton(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const UnrealMCP::FButtonParams& ParsedParams = ParamsResult.GetValue();
	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("widget_name"), ParsedParams.ButtonName);
	Response->SetStringField(TEXT("text"), ParsedParams.Text);
	return Response;
}