#pragma once

#include "CoreMinimal.h"

/**
 * Handles finding nodes in a blueprint graph by type and criteria
 */
class UNREALMCP_API FFindBlueprintNodes {
public:
	FFindBlueprintNodes() = default;
	~FFindBlueprintNodes() = default;

	/**
	 * Processes the provided JSON parameters to find blueprint nodes and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, node_type, event_name)
	 * @return A JSON object containing an array of node GUIDs or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
