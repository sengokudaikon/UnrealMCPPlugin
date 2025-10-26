#pragma once

#include "CoreMinimal.h"

class UNREALMCP_API FSetBlueprintProperty {
public:
	FSetBlueprintProperty() = default;

	~FSetBlueprintProperty() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
