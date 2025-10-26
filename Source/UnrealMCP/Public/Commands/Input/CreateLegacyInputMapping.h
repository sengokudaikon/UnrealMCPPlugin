#pragma once

#include "CoreMinimal.h"

/**
 * Handles creation of legacy input action mappings
 */
class UNREALMCP_API FCreateLegacyInputMapping {
public:
	FCreateLegacyInputMapping() = default;
	~FCreateLegacyInputMapping() = default;

	/**
	 * Processes the provided JSON parameters to create a legacy input mapping and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (action_name, key, shift, ctrl, alt, cmd)
	 * @return A JSON object containing details of the created mapping or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};