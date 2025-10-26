#pragma once

#include "CoreMinimal.h"

/**
 * Handles removing Input Mapping Contexts from the player's input subsystem at runtime
 */
class UNREALMCP_API FRemoveMappingContext {
public:
	FRemoveMappingContext() = default;
	~FRemoveMappingContext() = default;

	/**
	 * Processes the provided JSON parameters to remove a mapping context at runtime and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (context_path)
	 * @return A JSON object confirming the context was removed or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
