#pragma once

#include "CoreMinimal.h"

/**
 * Handles adding function call nodes to a blueprint graph
 */
class UNREALMCP_API FAddBlueprintFunctionCall {
public:
	FAddBlueprintFunctionCall() = default;
	~FAddBlueprintFunctionCall() = default;

	/**
	 * Processes the provided JSON parameters to add a function call node and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, function_name, target, node_position, params)
	 * @return A JSON object containing the node ID or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
