#pragma once

#include "CoreMinimal.h"

/**
 * Handles clearing all Input Mapping Contexts from the player's input subsystem at runtime
 */
class UNREALMCP_API FClearAllMappingContexts {
public:
	FClearAllMappingContexts() = default;
	~FClearAllMappingContexts() = default;

	/**
	 * Processes the provided JSON parameters to clear all mapping contexts at runtime and returns a JSON response.
	 *
	 * @param Params The JSON object (no parameters required)
	 * @return A JSON object confirming contexts were cleared or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
