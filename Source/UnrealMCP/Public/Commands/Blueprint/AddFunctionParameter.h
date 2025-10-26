#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to add a parameter to a blueprint function.
	 * Adds an input parameter with specified name, type, and reference behavior.
	 */
	class UNREALMCP_API FAddFunctionParameterCommand {
	public:
		/**
		 * Execute the command to add a function parameter.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - function_name: Name of the function
		 *   - param_name: Name of the parameter to add
		 *   - param_type: Type of the parameter (bool, int, float, string, vector, rotator, etc.)
		 *   - is_reference (optional): Whether the parameter is passed by reference (default: false)
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
