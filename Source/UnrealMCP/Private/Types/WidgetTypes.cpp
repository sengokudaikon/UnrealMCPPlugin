#include "Types/WidgetTypes.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"

namespace UnrealMCP {
	auto FWidgetCreationParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FWidgetCreationParams> {
		if (!Json.IsValid()) {
			return TResult<FWidgetCreationParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FWidgetCreationParams Params;

		if (!Json->TryGetStringField(TEXT("name"), Params.Name)) {
			return TResult<FWidgetCreationParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'name' parameter"));
		}

		Json->TryGetStringField(TEXT("parent_class"), Params.ParentClass);

		FString PackagePath;
		if (Json->TryGetStringField(TEXT("path"), PackagePath)) {
			Params.PackagePath = PackagePath;
		}

		return TResult<FWidgetCreationParams>::Success(MoveTemp(Params));
	}

	auto FTextBlockParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FTextBlockParams> {
		if (!Json.IsValid()) {
			return TResult<FTextBlockParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FTextBlockParams Params;

		if (!Json->TryGetStringField(TEXT("widget_name"), Params.WidgetName)) {
			return TResult<FTextBlockParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'widget_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("text_block_name"), Params.TextBlockName)) {
			return TResult<FTextBlockParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'text_block_name' parameter"));
		}

		Json->TryGetStringField(TEXT("text"), Params.Text);

		if (Json->HasField(TEXT("position"))) {
			Params.Position = FCommonUtils::GetVector2DFromJson(Json, TEXT("position"));
		}

		if (Json->HasField(TEXT("size"))) {
			Params.Size = FCommonUtils::GetVector2DFromJson(Json, TEXT("size"));
		}

		if (Json->HasField(TEXT("font_size"))) {
			Params.FontSize = static_cast<int32>(Json->GetNumberField(TEXT("font_size")));
		}

		return TResult<FTextBlockParams>::Success(MoveTemp(Params));
	}

	auto FButtonParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FButtonParams> {
		if (!Json.IsValid()) {
			return TResult<FButtonParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FButtonParams Params;

		if (!Json->TryGetStringField(TEXT("widget_name"), Params.WidgetName)) {
			return TResult<FButtonParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'widget_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("button_name"), Params.ButtonName)) {
			return TResult<FButtonParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'button_name' parameter"));
		}

		Json->TryGetStringField(TEXT("text"), Params.Text);

		if (Json->HasField(TEXT("position"))) {
			Params.Position = FCommonUtils::GetVector2DFromJson(Json, TEXT("position"));
		}

		if (Json->HasField(TEXT("size"))) {
			Params.Size = FCommonUtils::GetVector2DFromJson(Json, TEXT("size"));
		}

		if (Json->HasField(TEXT("font_size"))) {
			Params.FontSize = static_cast<int32>(Json->GetNumberField(TEXT("font_size")));
		}

		return TResult<FButtonParams>::Success(MoveTemp(Params));
	}

	auto FWidgetEventBindingParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FWidgetEventBindingParams> {
		if (!Json.IsValid()) {
			return TResult<FWidgetEventBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FWidgetEventBindingParams Params;

		if (!Json->TryGetStringField(TEXT("widget_name"), Params.WidgetName)) {
			return TResult<FWidgetEventBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'widget_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("widget_component_name"), Params.WidgetComponentName)) {
			return TResult<FWidgetEventBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'widget_component_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("event_name"), Params.EventName)) {
			return TResult<FWidgetEventBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'event_name' parameter"));
		}

		if (Json->TryGetStringField(TEXT("function_name"), Params.FunctionName)) {
			// Use provided function name
		}
		else {
			// Default function name
			Params.FunctionName = Params.WidgetComponentName + TEXT("_") + Params.EventName;
		}

		return TResult<FWidgetEventBindingParams>::Success(MoveTemp(Params));
	}

	auto FTextBlockBindingParams::FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FTextBlockBindingParams> {
		if (!Json.IsValid()) {
			return TResult<FTextBlockBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FTextBlockBindingParams Params;

		if (!Json->TryGetStringField(TEXT("widget_name"), Params.WidgetName)) {
			return TResult<FTextBlockBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'widget_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("text_block_name"), Params.TextBlockName)) {
			return TResult<FTextBlockBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'text_block_name' parameter"));
		}

		if (!Json->TryGetStringField(TEXT("binding_property"), Params.BindingProperty)) {
			return TResult<FTextBlockBindingParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'binding_property' parameter"));
		}

		Json->TryGetStringField(TEXT("binding_type"), Params.BindingType);

		return TResult<FTextBlockBindingParams>::Success(MoveTemp(Params));
	}

	auto FAddWidgetToViewportParams::FromJson(
		const TSharedPtr<FJsonObject>& Json) -> TResult<FAddWidgetToViewportParams> {
		if (!Json.IsValid()) {
			return TResult<FAddWidgetToViewportParams>::Failure(EErrorCode::InvalidInput, TEXT("Invalid JSON object"));
		}

		FAddWidgetToViewportParams Params;

		if (!Json->TryGetStringField(TEXT("widget_name"), Params.WidgetName)) {
			return TResult<FAddWidgetToViewportParams>::Failure(EErrorCode::InvalidInput, TEXT("Missing 'widget_name' parameter"));
		}

		if (Json->HasField(TEXT("z_order"))) {
			Params.ZOrder = static_cast<int32>(Json->GetNumberField(TEXT("z_order")));
		}

		return TResult<FAddWidgetToViewportParams>::Success(MoveTemp(Params));
	}
}
