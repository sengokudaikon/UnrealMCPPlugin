#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	class UNREALMCP_API FListBlueprintsCommand {
	public:
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

} // namespace UnrealMCP
