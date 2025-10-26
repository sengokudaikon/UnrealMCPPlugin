#pragma once

#include "CoreMinimal.h"

/**
 * Handles removing key mappings from Input Mapping Context assets
 */
class UNREALMCP_API FRemoveEnhancedInputMapping {
public:
	FRemoveEnhancedInputMapping() = default;
	~FRemoveEnhancedInputMapping() = default;

	/**
	 * Processes the provided JSON parameters to remove a key mapping from a context and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (context_path, action_path)
	 * @return A JSON object confirming the mapping was removed or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
