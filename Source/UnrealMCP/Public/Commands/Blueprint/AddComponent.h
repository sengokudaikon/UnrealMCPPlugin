#pragma once

#include "CoreMinimal.h"

class UNREALMCP_API FAddComponent {
public:
	FAddComponent() = default;

	~FAddComponent() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
