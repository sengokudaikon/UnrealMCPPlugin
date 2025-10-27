#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Retrieves the components of a specified blueprint.
	 */
	class UNREALMCP_API FGetBlueprintComponentsCommand {
	public:
		FGetBlueprintComponentsCommand() = default;

		~FGetBlueprintComponentsCommand() = default;

		/**
		 * Processes the provided JSON parameters to get blueprint components and returns a JSON response.
		 *
		 * @param Params The JSON object containing the blueprint name parameter.
		 * @return A JSON object containing the blueprint components or an error response if the operation fails.
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
