#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Core/Result.h"

namespace UnrealMCP {
	/**
	 * Parameters for creating a UMG widget blueprint
	 */
	struct FWidgetCreationParams {
		FString Name;
		FString ParentClass = TEXT("UserWidget");
		FString PackagePath = TEXT("/Game/UI");

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FWidgetCreationParams>;
	};

	/**
	 * Parameters for adding a text block to a widget
	 */
	struct FTextBlockParams {
		FString WidgetName;
		FString TextBlockName;
		FString Text = TEXT("");
		TOptional<FVector2D> Position;
		TOptional<FVector2D> Size;
		int32 FontSize = 12;
		TOptional<FLinearColor> Color;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FTextBlockParams>;
	};

	/**
	 * Parameters for adding a button to a widget
	 */
	struct FButtonParams {
		FString WidgetName;
		FString ButtonName;
		FString Text = TEXT("");
		TOptional<FVector2D> Position;
		TOptional<FVector2D> Size;
		int32 FontSize = 12;
		TOptional<FLinearColor> TextColor;
		TOptional<FLinearColor> BackgroundColor;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FButtonParams>;
	};

	/**
	 * Parameters for binding a widget event
	 */
	struct FWidgetEventBindingParams {
		FString WidgetName;
		FString WidgetComponentName;
		FString EventName;
		FString FunctionName;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FWidgetEventBindingParams>;
	};

	/**
	 * Parameters for setting a text block binding
	 */
	struct FTextBlockBindingParams {
		FString WidgetName;
		FString TextBlockName;
		FString BindingProperty;
		FString BindingType = TEXT("Text");

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FTextBlockBindingParams>;
	};

	/**
	 * Parameters for adding a widget to viewport
	 */
	struct FAddWidgetToViewportParams {
		FString WidgetName;
		int32 ZOrder = 0;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FAddWidgetToViewportParams>;
	};
}
