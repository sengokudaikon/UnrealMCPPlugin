#pragma once

#include "CoreMinimal.h"

class UNREALMCP_API FSetPawnProperties {
public:
	FSetPawnProperties() = default;

	~FSetPawnProperties() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
