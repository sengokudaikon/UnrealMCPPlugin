#pragma once

#include "CoreMinimal.h"

/**
 * Handles deleting actors from the level
 * Removes actors by name and returns confirmation
 */
class UNREALMCP_API FDeleteActor {
public:
	FDeleteActor() = default;
	~FDeleteActor() = default;

	/**
	 * Deletes an actor from the level by name
	 *
	 * @param Params The JSON object containing parameters (name)
	 * @return A JSON object containing deletion confirmation or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};