#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	class UNREALMCP_API FSetPawnProperties {
	public:
		FSetPawnProperties() = default;

		~FSetPawnProperties() = default;

		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}
