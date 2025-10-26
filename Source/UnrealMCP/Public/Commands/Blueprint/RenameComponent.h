#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to rename a component in a blueprint.
	 * Updates the component variable name and any references.
	 */
	class UNREALMCP_API FRenameComponentCommand {
	public:
		/**
		 * Execute the command to rename a component.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - old_name: Current name of the component
		 *   - new_name: New name for the component
		 * @return JSON object with success status
		 */
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

} // namespace UnrealMCP
