#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to duplicate a blueprint asset.
	 * Creates a copy of the blueprint with a new name.
	 */
	class UNREALMCP_API FDuplicateBlueprintCommand {
	public:
		/**
		 * Execute the command to duplicate a blueprint.
		 *
		 * @param Params JSON object with:
		 *   - source_name: Name or path of the blueprint to duplicate
		 *   - new_name: Name for the new blueprint
		 *   - path: Optional package path (defaults to /Game/Blueprints/)
		 * @return JSON object with success status and new blueprint path
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
