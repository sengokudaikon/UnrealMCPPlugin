#pragma once

#include "CoreMinimal.h"

/**
 * Handles setting actor transform (position, rotation, scale)
 * Updates actor's transform in the editor
 */
class UNREALMCP_API FSetActorTransform {
public:
	FSetActorTransform() = default;
	~FSetActorTransform() = default;

	/**
	 * Sets the transform of an actor by name
	 *
	 * @param Params The JSON object containing parameters (name, location, rotation, scale)
	 * @return A JSON object containing updated actor details or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};