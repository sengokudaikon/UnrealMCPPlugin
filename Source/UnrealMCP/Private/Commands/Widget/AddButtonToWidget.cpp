#include "Commands/UMG/AddButtonToWidget.h"
#include "Core/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"
#include "Components/Button.h"

auto FAddButtonToWidget::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FButtonParams> ParamsResult = UnrealMCP::FButtonParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	if (const auto Result = UnrealMCP::FWidgetService::AddButton(ParamsResult.GetValue()); Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const auto& ParsedParams = ParamsResult.GetValue();
	
	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("widget_name"), ParsedParams.ButtonName);
		Data->SetStringField(TEXT("text"), ParsedParams.Text);
	});
}
