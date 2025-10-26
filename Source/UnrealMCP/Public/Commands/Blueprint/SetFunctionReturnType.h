#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to set the return type of a blueprint function.
	 * Configures the output pin on the function's result node.
	 */
	class UNREALMCP_API FSetFunctionReturnTypeCommand {
	public:
		/**
		 * Execute the command to set function return type.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - function_name: Name of the function
		 *   - return_type: Type of the return value (bool, int, float, string, vector, rotator, etc.)
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
