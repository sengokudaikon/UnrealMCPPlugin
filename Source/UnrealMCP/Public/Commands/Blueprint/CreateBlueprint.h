#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for creating blueprints dynamically.
 */
class UNREALMCP_API FCreateBlueprint {
public:
	FCreateBlueprint() = default;
	~FCreateBlueprint() = default;

	/**
	 * Processes the provided JSON parameters to create a blueprint and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters required for blueprint creation.
	 * @return A JSON object containing details of the created blueprint or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};