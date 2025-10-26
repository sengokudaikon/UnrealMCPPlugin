#pragma once

#include "CoreMinimal.h"

/**
 * Handles adding legacy input action nodes to a blueprint graph
 */
class UNREALMCP_API FAddBlueprintInputActionNode {
public:
	FAddBlueprintInputActionNode() = default;
	~FAddBlueprintInputActionNode() = default;

	/**
	 * Processes the provided JSON parameters to add an input action node and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, action_name, node_position)
	 * @return A JSON object containing the node ID or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
