#include "Commands/UMG/AddTextBlockToWidget.h"
#include "Commands/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"
#include "Components/TextBlock.h"

auto FAddTextBlockToWidget::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FTextBlockParams> ParamsResult =
		UnrealMCP::FTextBlockParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::TResult<UTextBlock*> Result =
		UnrealMCP::FWidgetService::AddTextBlock(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const UnrealMCP::FTextBlockParams& ParsedParams = ParamsResult.GetValue();
	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("widget_name"), ParsedParams.TextBlockName);
	Response->SetStringField(TEXT("text"), ParsedParams.Text);
	return Response;
}