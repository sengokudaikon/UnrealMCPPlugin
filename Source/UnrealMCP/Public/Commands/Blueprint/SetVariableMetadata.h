#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to set metadata properties on a blueprint variable.
	 * Configures tooltip, category, visibility, and editability.
	 */
	class UNREALMCP_API FSetVariableMetadataCommand {
	public:
		/**
		 * Execute the command to set variable metadata.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - variable_name: Name of the variable
		 *   - tooltip (optional): Tooltip text
		 *   - category (optional): Category name
		 *   - expose_on_spawn (optional): Whether to expose on spawn
		 *   - instance_editable (optional): Whether editable per-instance
		 *   - blueprint_read_only (optional): Whether read-only in BP
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}
