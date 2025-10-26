#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command to delete a blueprint asset.
	 * Removes the blueprint from the content browser and cleans up references.
	 */
	class UNREALMCP_API FDeleteBlueprintCommand {
	public:
		/**
		 * Execute the command to delete a blueprint.
		 *
		 * @param Params JSON object with:
		 *   - blueprint_name: Name or path of the blueprint to delete
		 * @return JSON object with success status
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
