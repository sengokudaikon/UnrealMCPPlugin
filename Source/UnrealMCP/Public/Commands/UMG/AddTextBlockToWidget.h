#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for adding Text Block widgets to UMG Widget Blueprints.
 */
class UNREALMCP_API FAddTextBlockToWidget {
public:
	FAddTextBlockToWidget() = default;
	~FAddTextBlockToWidget() = default;

	/**
	 * Processes the provided JSON parameters to add a Text Block to a widget blueprint and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters required for adding the Text Block.
	 * @return A JSON object containing details of the added Text Block or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};