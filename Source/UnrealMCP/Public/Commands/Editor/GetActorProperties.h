#pragma once

#include "CoreMinimal.h"

/**
 * Handles getting actor properties
 * Returns detailed property information for a specific actor
 */
class UNREALMCP_API FGetActorProperties {
public:
	FGetActorProperties() = default;
	~FGetActorProperties() = default;

	/**
	 * Gets properties of an actor by name
	 *
	 * @param Params The JSON object containing parameters (name)
	 * @return A JSON object containing actor properties or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};