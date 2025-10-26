#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to remove a function from a blueprint.
	 * Deletes the function graph and all associated nodes.
	 */
	class UNREALMCP_API FRemoveFunctionCommand {
	public:
		/**
		 * Execute the command to remove a function.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - function_name: Name of the function to remove
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
