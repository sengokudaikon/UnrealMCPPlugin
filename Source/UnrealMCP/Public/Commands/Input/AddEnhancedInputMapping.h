#pragma once

#include "CoreMinimal.h"

/**
 * Handles adding key mappings to Input Mapping Context assets
 */
class UNREALMCP_API FAddEnhancedInputMapping {
public:
	FAddEnhancedInputMapping() = default;
	~FAddEnhancedInputMapping() = default;

	/**
	 * Processes the provided JSON parameters to add a key mapping to a context and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (context_path, action_path, key)
	 * @return A JSON object confirming the mapping was added or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
