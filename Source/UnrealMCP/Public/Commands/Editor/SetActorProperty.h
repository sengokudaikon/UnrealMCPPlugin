#pragma once

#include "CoreMinimal.h"

/**
 * Handles setting actor properties
 * Updates a specific property on an actor
 */
class UNREALMCP_API FSetActorProperty {
public:
	FSetActorProperty() = default;
	~FSetActorProperty() = default;

	/**
	 * Sets a property of an actor by name
	 *
	 * @param Params The JSON object containing parameters (name, property_name, property_value)
	 * @return A JSON object containing confirmation or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};