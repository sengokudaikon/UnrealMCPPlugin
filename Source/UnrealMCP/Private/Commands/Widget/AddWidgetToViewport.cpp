#include "Commands/UMG/AddWidgetToViewport.h"
#include "Core/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"
#include "UObject/Class.h"

auto FAddWidgetToViewport::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FAddWidgetToViewportParams> ParamsResult =
		UnrealMCP::FAddWidgetToViewportParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	UnrealMCP::TResult<UClass*> Result =
		UnrealMCP::FWidgetService::GetWidgetClass(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const auto& [WidgetName, ZOrder] = ParamsResult.GetValue();
	const UClass* WidgetClass = Result.GetValue();

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("widget_name"), WidgetName);
		Data->SetStringField(TEXT("class_path"), WidgetClass ? WidgetClass->GetPathName() : TEXT(""));
		Data->SetNumberField(TEXT("z_order"), ZOrder);
		Data->SetStringField(
			TEXT("note"),
			TEXT("Widget class ready. Use CreateWidget and AddToViewport nodes in Blueprint to display in game.")
		);
	});
}