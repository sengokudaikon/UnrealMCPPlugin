#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {
	/**
	 * Retrieves the path of a specified blueprint.
	 */
	class UNREALMCP_API FGetBlueprintPathCommand {
	public:
		FGetBlueprintPathCommand() = default;

		~FGetBlueprintPathCommand() = default;

		/**
		 * Processes the provided JSON parameters to get blueprint path and returns a JSON response.
		 *
		 * @param Params The JSON object containing the blueprint name parameter.
		 * @return A JSON object containing the blueprint path or an error response if the operation fails.
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}
