#include "Commands/Widget/AddButtonToWidget.h"
#include "Components/Button.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/WidgetService.h"

namespace UnrealMCP {

	auto FAddButtonToWidget::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		TResult<FButtonParams> ParamsResult = FButtonParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		if (const auto Result = FWidgetService::AddButton(ParamsResult.GetValue()); Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		const auto& ParsedParams = ParamsResult.GetValue();

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("widget_name"), ParsedParams.ButtonName);
			Data->SetStringField(TEXT("text"), ParsedParams.Text);
		});
	}
}
