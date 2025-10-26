#pragma once

#include "CoreMinimal.h"

/**
 * Handles creation of Input Mapping Context assets
 */
class UNREALMCP_API FCreateInputMappingContext {
public:
	FCreateInputMappingContext() = default;
	~FCreateInputMappingContext() = default;

	/**
	 * Processes the provided JSON parameters to create an input mapping context and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (name, path)
	 * @return A JSON object containing details of the created mapping context or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
