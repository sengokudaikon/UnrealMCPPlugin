#pragma once

#include "CoreMinimal.h"

/**
 * Handles adding variables to a blueprint
 */
class UNREALMCP_API FAddBlueprintVariable {
public:
	FAddBlueprintVariable() = default;
	~FAddBlueprintVariable() = default;

	/**
	 * Processes the provided JSON parameters to add a variable to a blueprint and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, variable_name, variable_type, is_exposed)
	 * @return A JSON object containing variable details or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
