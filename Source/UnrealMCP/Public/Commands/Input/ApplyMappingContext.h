#pragma once

#include "CoreMinimal.h"

/**
 * Handles applying Input Mapping Contexts to the player's input subsystem at runtime
 */
class UNREALMCP_API FApplyMappingContext {
public:
	FApplyMappingContext() = default;
	~FApplyMappingContext() = default;

	/**
	 * Processes the provided JSON parameters to apply a mapping context at runtime and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (context_path, priority)
	 * @return A JSON object confirming the context was applied or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
