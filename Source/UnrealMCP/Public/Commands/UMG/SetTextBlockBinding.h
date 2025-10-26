#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for setting up text block bindings in UMG Widget Blueprints.
 */
class UNREALMCP_API FSetTextBlockBinding {
public:
	FSetTextBlockBinding() = default;
	~FSetTextBlockBinding() = default;

	/**
	 * Processes the provided JSON parameters to set up text block binding and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters required for setting up the text block binding.
	 * @return A JSON object containing details of the text block binding or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};