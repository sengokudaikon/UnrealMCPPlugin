#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for adding widget instances to the game viewport.
 */
class UNREALMCP_API FAddWidgetToViewport {
public:
	FAddWidgetToViewport() = default;
	~FAddWidgetToViewport() = default;

	/**
	 * Processes the provided JSON parameters to add a widget to the viewport and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters required for adding the widget to viewport.
	 * @return A JSON object containing details of the widget viewport addition or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};