#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for adding Button widgets to UMG Widget Blueprints.
 */
class UNREALMCP_API FAddButtonToWidget {
public:
	FAddButtonToWidget() = default;
	~FAddButtonToWidget() = default;

	/**
	 * Processes the provided JSON parameters to add a Button to a widget blueprint and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters required for adding the Button.
	 * @return A JSON object containing details of the added Button or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};