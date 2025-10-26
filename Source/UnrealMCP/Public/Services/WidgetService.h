#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"
#include "Core/MCPTypes.h"
#include "Json.h"

class UWidgetBlueprint;
class UWidget;
class UTextBlock;
class UButton;
class UCanvasPanelSlot;

namespace UnrealMCP
{
	/**
	 * High-level service for UMG widget operations
	 * Coordinates between widget creation, manipulation, and viewport integration
	 *
	 * All operations return TResult<T> for consistency. JSON conversion is handled
	 * at the command handler layer, not within the service itself.
	 */
	class UNREALMCP_API FWidgetService
	{
	public:
		static TResult<UWidgetBlueprint*> CreateWidget(const FWidgetCreationParams& Params);

		static TResult<UTextBlock*> AddTextBlock(const FTextBlockParams& Params);

		static TResult<UButton*> AddButton(const FButtonParams& Params);

		static FVoidResult BindWidgetEvent(const FWidgetEventBindingParams& Params);

		static FVoidResult SetTextBlockBinding(const FTextBlockBindingParams& Params);

		static TResult<UClass*> GetWidgetClass(const FAddWidgetToViewportParams& Params);

	private:
		/**
		 * Ensures an asset name is unique by appending a number if necessary
		 * @param BaseName The desired base name for the asset
		 * @param PackagePath The package path where the asset will be created
		 * @return A unique asset name that doesn't conflict with existing assets
		 */
		static FString EnsureUniqueAssetName(const FString& BaseName, const FString& PackagePath);

		/**
		 * Resolves a widget name to a full asset path
		 * Supports both full paths (e.g., "/Game/UI/MyWidget") and short names (e.g., "MyWidget")
		 * Short names are assumed to be in /Game/UI directory
		 */
		static FString ResolveWidgetPath(const FString& WidgetName);

		/**
		 * Validates that a widget blueprint has a valid Canvas Panel root
		 * Required for adding widgets to the hierarchy
		 */
		static FVoidResult ValidateCanvasRoot(const UWidgetBlueprint* WidgetBlueprint);

		/**
		 * Applies position and size properties to a canvas panel slot if parameters are set
		 */
		static void ApplyCanvasSlotTransform(::UCanvasPanelSlot* Slot, const TOptional<FVector2D>& Position, const TOptional<FVector2D>& Size);
	};
}
