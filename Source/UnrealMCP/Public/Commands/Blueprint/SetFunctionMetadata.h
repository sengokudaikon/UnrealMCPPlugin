#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to set metadata properties on a blueprint function.
	 * Configures category, tooltip, and pure flag.
	 */
	class UNREALMCP_API FSetFunctionMetadataCommand {
	public:
		/**
		 * Execute the command to set function metadata.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - function_name: Name of the function
		 *   - category (optional): Function category for organization
		 *   - tooltip (optional): Function description/tooltip
		 *   - pure (optional): Whether the function is pure (no side effects)
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
