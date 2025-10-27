#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	class UNREALMCP_API FAddComponent {
	public:
		FAddComponent() = default;

		~FAddComponent() = default;

		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

}
