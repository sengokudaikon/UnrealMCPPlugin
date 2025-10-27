#include "Commands/Widget/AddTextBlockToWidget.h"
#include "Components/TextBlock.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/WidgetService.h"

namespace UnrealMCP {

	auto FAddTextBlockToWidget::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		TResult<FTextBlockParams> ParamsResult =
			FTextBlockParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const TResult<UTextBlock*> Result =
			FWidgetService::AddTextBlock(ParamsResult.GetValue());

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		const FTextBlockParams& ParsedParams = ParamsResult.GetValue();
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("widget_name"), ParsedParams.TextBlockName);
			Data->SetStringField(TEXT("text"), ParsedParams.Text);
		});
	}
}
