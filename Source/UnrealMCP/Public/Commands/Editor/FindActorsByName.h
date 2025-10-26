#pragma once

#include "CoreMinimal.h"

/**
 * Handles finding actors by name pattern
 * Returns actors whose names contain the specified pattern
 */
class UNREALMCP_API FFindActorsByName {
public:
	FFindActorsByName() = default;
	~FFindActorsByName() = default;

	/**
	 * Returns a JSON array containing actors matching the name pattern
	 *
	 * @param Params The JSON object containing parameters (pattern)
	 * @return A JSON object containing the matching actors array or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};