#pragma once

#include "CoreMinimal.h"

/**
 * Handles adding event nodes to a blueprint graph
 */
class UNREALMCP_API FAddBlueprintEvent {
public:
	FAddBlueprintEvent() = default;
	~FAddBlueprintEvent() = default;

	/**
	 * Processes the provided JSON parameters to add an event node and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (blueprint_name, event_name, node_position)
	 * @return A JSON object containing the node ID or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
