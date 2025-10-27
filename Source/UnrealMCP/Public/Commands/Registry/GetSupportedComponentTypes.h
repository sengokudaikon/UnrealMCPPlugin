#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command: Get all supported component types.
	 */
	class UNREALMCP_API FGetSupportedComponentTypesCommand {
	public:
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
