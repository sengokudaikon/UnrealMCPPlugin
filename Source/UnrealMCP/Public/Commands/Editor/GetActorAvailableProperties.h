#pragma once

#include "CoreMinimal.h"

namespace UnrealMCP {
	/**
	 * Handles getting available properties for an actor
	 * Returns all settable properties for a specific actor class
	 */
	class UNREALMCP_API FGetActorAvailableProperties {
	public:
		FGetActorAvailableProperties() = default;

		~FGetActorAvailableProperties() = default;

		/**
		 * Gets all available properties for an actor by name
		 *
		 * @param Params The JSON object containing parameters (name)
		 * @return A JSON object containing available properties or an error response
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}