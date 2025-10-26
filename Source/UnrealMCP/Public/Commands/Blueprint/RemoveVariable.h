#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to remove a variable from a blueprint.
	 * Deletes the variable and updates all references.
	 */
	class UNREALMCP_API FRemoveVariableCommand {
	public:
		/**
		 * Execute the command to remove a variable.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - variable_name: Name of the variable to remove
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
