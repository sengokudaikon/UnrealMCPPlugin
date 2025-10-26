#pragma once

#include "CoreMinimal.h"
#include "Json.h"
namespace UnrealMCP {
	/**
	 * Retrieves the variables of a specified blueprint.
	 */
	class UNREALMCP_API FGetBlueprintVariablesCommand {
	public:
		FGetBlueprintVariablesCommand() = default;
		~FGetBlueprintVariablesCommand() = default;

		/**
		 * Processes the provided JSON parameters to get blueprint variables and returns a JSON response.
		 *
		 * @param Params The JSON object containing the blueprint name parameter.
		 * @return A JSON object containing the blueprint variables or an error response if the operation fails.
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}