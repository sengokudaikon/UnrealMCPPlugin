#pragma once

#include "CoreMinimal.h"

/**
 * Handles getting all actors in the current level
 * Returns a list of actors with their basic information
 */
class UNREALMCP_API FGetActorsInLevel {
public:
	FGetActorsInLevel() = default;
	~FGetActorsInLevel() = default;

	/**
	 * Returns a JSON array containing all actors in the current level
	 *
	 * @param Params The JSON object containing parameters (none required)
	 * @return A JSON object containing the actors array or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};