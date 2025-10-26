#pragma once

#include "CoreMinimal.h"

class UNREALMCP_API FSetPhysicsProperties {
public:
	FSetPhysicsProperties() = default;

	~FSetPhysicsProperties() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
