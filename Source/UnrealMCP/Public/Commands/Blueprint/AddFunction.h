#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to add a custom function to a blueprint.
	 * Creates a new function graph with entry and result nodes.
	 */
	class UNREALMCP_API FAddFunctionCommand {
	public:
		/**
		 * Execute the command to add a function.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - function_name: Name of the function to create
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
