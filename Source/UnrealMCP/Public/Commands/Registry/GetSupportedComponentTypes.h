#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command: Get all supported component types.
	 */
	class UNREALMCP_API FGetSupportedComponentTypesCommand {
	public:
		static TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Params);
	};

} // namespace UnrealMCP
