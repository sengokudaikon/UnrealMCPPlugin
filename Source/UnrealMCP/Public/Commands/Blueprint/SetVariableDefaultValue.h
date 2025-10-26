#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to set the default value of a blueprint variable.
	 * Supports bool, int, float, string, Vector, Rotator, and other basic types.
	 */
	class UNREALMCP_API FSetVariableDefaultValueCommand {
	public:
		/**
		 * Execute the command to set variable default value.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - variable_name: Name of the variable
		 *   - value: Default value (type depends on variable type)
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}
