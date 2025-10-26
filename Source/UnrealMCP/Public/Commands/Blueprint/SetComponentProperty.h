#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

class UNREALMCP_API FSetComponentProperty {
public:
	FSetComponentProperty() = default;

	~FSetComponentProperty() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};

}
