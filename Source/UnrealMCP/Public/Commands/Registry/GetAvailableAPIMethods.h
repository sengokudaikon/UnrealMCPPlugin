#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	/**
	 * Command: Get all available API methods organized by category.
	 */
	class UNREALMCP_API FGetAvailableAPIMethodsCommand {
	public:
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
