#pragma once

#include "CoreMinimal.h"

class UNREALMCP_API FSetComponentProperty {
public:
	FSetComponentProperty() = default;

	~FSetComponentProperty() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
