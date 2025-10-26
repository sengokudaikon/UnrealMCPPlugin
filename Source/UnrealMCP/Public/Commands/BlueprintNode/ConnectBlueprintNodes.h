#pragma once

#include "CoreMinimal.h"

/**
 * Handles connecting blueprint nodes via their pins
 */
class UNREALMCP_API FConnectBlueprintNodes {
public:
	FConnectBlueprintNodes() = default;
	~FConnectBlueprintNodes() = default;

	/**
	 * Processes the provided JSON parameters to connect two blueprint nodes and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, source_node_id, target_node_id, source_pin, target_pin)
	 * @return A JSON object confirming the connection or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
