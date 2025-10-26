#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command: Get all available API methods organized by category.
	 */
	class UNREALMCP_API FGetAvailableAPIMethodsCommand {
	public:
		static TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Params);
	};

} // namespace UnrealMCP
