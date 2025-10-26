#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to rename a variable in a blueprint.
	 * Renames the variable and updates all references.
	 */
	class UNREALMCP_API FRenameVariableCommand {
	public:
		/**
		 * Execute the command to rename a variable.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - old_name: Current name of the variable
		 *   - new_name: New name for the variable
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
