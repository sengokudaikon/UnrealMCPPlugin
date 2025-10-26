#pragma once

#include "CoreMinimal.h"

/**
 * Handles adding self reference nodes to a blueprint graph
 */
class UNREALMCP_API FAddBlueprintSelfReference {
public:
	FAddBlueprintSelfReference() = default;
	~FAddBlueprintSelfReference() = default;

	/**
	 * Processes the provided JSON parameters to add a self reference node and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, node_position)
	 * @return A JSON object containing the node ID or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
