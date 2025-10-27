#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	class UNREALMCP_API FSetBlueprintProperty {
	public:
		FSetBlueprintProperty() = default;

		~FSetBlueprintProperty() = default;

		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
