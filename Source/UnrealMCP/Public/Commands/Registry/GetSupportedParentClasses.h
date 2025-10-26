#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command: Get all supported parent classes for Blueprint creation.
	 */
	class UNREALMCP_API FGetSupportedParentClassesCommand {
	public:
		/**
		 * Execute the command.
		 *
		 * @param Params Empty JSON object (no parameters needed)
		 * @return JSON response with array of class names
		 */
		static TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Params);
	};

} // namespace UnrealMCP
