#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to get all custom functions defined in a blueprint.
	 * Returns information about function signatures, parameters, and return types.
	 */
	class UNREALMCP_API FGetBlueprintFunctionsCommand {
	public:
		/**
		 * Execute the command to get blueprint functions.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 * @return JSON object with success status and array of function info
		 */
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

} // namespace UnrealMCP
