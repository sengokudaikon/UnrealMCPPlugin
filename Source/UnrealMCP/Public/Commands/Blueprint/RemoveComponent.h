#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to remove a component from a blueprint.
	 * Deletes the specified component and cleans up any references.
	 */
	class UNREALMCP_API FRemoveComponentCommand {
	public:
		/**
		 * Execute the command to remove a component.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - component_name: Name of the component to remove
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
