#pragma once

#include "CoreMinimal.h"
#include "Json.h"
namespace UnrealMCP {
	/**
	 * Retrieves detailed information about a specified blueprint.
	 */
	class UNREALMCP_API FGetBlueprintInfoCommand {
	public:
		FGetBlueprintInfoCommand() = default;
		~FGetBlueprintInfoCommand() = default;

		/**
		 * Processes the provided JSON parameters to get blueprint information and returns a JSON response.
		 *
		 * @param Params The JSON object containing the blueprint name parameter.
		 * @return A JSON object containing the blueprint information or an error response if the operation fails.
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}