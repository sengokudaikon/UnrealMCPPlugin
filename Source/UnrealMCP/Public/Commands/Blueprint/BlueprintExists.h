#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

/**
 * Checks if a blueprint with the given name exists.
 */
class UNREALMCP_API FBlueprintExistsCommand {
public:
	FBlueprintExistsCommand() = default;
	~FBlueprintExistsCommand() = default;

	/**
	 * Processes the provided JSON parameters to check if a blueprint exists and returns a JSON response.
	 *
	 * @param Params The JSON object containing the blueprint name parameter.
	 * @return A JSON object containing whether the blueprint exists or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};

}