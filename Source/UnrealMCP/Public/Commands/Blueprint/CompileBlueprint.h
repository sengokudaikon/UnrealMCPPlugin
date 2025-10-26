#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for compiling blueprints dynamically.
 */
class UNREALMCP_API FCompileBlueprint {
public:
	FCompileBlueprint() = default;

	~FCompileBlueprint() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
