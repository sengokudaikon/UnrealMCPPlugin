#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to set the transform (location, rotation, scale) of a component in a blueprint.
	 * Updates the component template's relative transform.
	 */
	class UNREALMCP_API FSetComponentTransformCommand {
	public:
		/**
		 * Execute the command to set component transform.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name of the blueprint
		 *   - component_name: Name of the component
		 *   - location: Optional [x, y, z] array for location
		 *   - rotation: Optional [pitch, yaw, roll] array for rotation
		 *   - scale: Optional [x, y, z] array for scale
		 * @return JSON object with success status
		 */
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

} // namespace UnrealMCP
