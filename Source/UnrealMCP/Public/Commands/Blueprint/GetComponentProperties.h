#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to get all properties of a specific component in a blueprint.
	 * Returns detailed information about the component including its type,
	 * transform, and all configurable properties.
	 */
	class UNREALMCP_API FGetComponentPropertiesCommand {
	public:
		/**
		 * Execute the command to get component properties.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - component_name: Name of the component to query
		 * @return JSON object with success status and component properties
		 */
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

} // namespace UnrealMCP
