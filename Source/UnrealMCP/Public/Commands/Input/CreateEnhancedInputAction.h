#pragma once

#include "CoreMinimal.h"

/**
 * Handles creation of Enhanced Input Action assets
 */
class UNREALMCP_API FCreateEnhancedInputAction {
public:
	FCreateEnhancedInputAction() = default;
	~FCreateEnhancedInputAction() = default;

	/**
	 * Processes the provided JSON parameters to create an input action and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (name, value_type, path)
	 * @return A JSON object containing details of the created input action or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
