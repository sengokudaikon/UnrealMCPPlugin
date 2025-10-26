#pragma once

#include "CoreMinimal.h"

/**
 * Handles adding a self component reference node to a blueprint graph
 */
class UNREALMCP_API FAddBlueprintGetSelfComponentReference {
public:
	FAddBlueprintGetSelfComponentReference() = default;
	~FAddBlueprintGetSelfComponentReference() = default;

	/**
	 * Processes the provided JSON parameters to add a self component reference node and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, component_name, node_position)
	 * @return A JSON object containing the node ID or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
