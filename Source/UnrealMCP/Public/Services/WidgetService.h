#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"

class UWidgetBlueprint;
class UWidget;
class UTextBlock;
class UButton;
class UCanvasPanelSlot;

namespace UnrealMCP {
	/**
	 * High-level service for UMG widget operations
	 * Coordinates between widget creation, manipulation, and viewport integration
	 *
	 * All operations return TResult<T> for consistency. JSON conversion is handled
	 * at the command handler layer, not within the service itself.
	 */
	class UNREALMCP_API FWidgetService {
	public:
		static auto CreateWidget(const FWidgetCreationParams& Params) -> TResult<UWidgetBlueprint*>;

		static auto AddTextBlock(const FTextBlockParams& Params) -> TResult<UTextBlock*>;

		static auto AddButton(const FButtonParams& Params) -> TResult<UButton*>;

		static auto BindWidgetEvent(const FWidgetEventBindingParams& Params) -> FVoidResult;

		static auto SetTextBlockBinding(const FTextBlockBindingParams& Params) -> FVoidResult;

		static auto GetWidgetClass(const FAddWidgetToViewportParams& Params) -> TResult<UClass*>;

	private:
		/**
		 * Ensures an asset name is unique by appending a number if necessary
		 * @param BaseName The desired base name for the asset
		 * @param PackagePath The package path where the asset will be created
		 * @return A unique asset name that doesn't conflict with existing assets
		 */
		static auto EnsureUniqueAssetName(const FString& BaseName, const FString& PackagePath) -> FString;

		/**
		 * Resolves a widget name to a full asset path
		 * Supports both full paths (e.g., "/Game/UI/MyWidget") and short names (e.g., "MyWidget")
		 * Short names are assumed to be in /Game/UI directory
		 */
		static auto ResolveWidgetPath(const FString& WidgetName) -> FString;

		/**
		 * Validates that a widget blueprint has a valid Canvas Panel root
		 * Required for adding widgets to the hierarchy
		 */
		static auto ValidateCanvasRoot(const UWidgetBlueprint* WidgetBlueprint) -> FVoidResult;

		/**
		 * Applies position and size properties to a canvas panel slot if parameters are set
		 */
		static auto ApplyCanvasSlotTransform(UCanvasPanelSlot* Slot,
		                                     const TOptional<FVector2D>& Position,
		                                     const TOptional<FVector2D>& Size) -> void;
	};
}
