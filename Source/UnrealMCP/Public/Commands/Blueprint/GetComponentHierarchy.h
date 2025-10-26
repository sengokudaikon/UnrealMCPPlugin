#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to get the component hierarchy tree for a blueprint.
	 * Returns the parent-child relationships between all components.
	 */
	class UNREALMCP_API FGetComponentHierarchyCommand {
	public:
		/**
		 * Execute the command to get component hierarchy.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 * @return JSON object with success status and hierarchy tree
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
