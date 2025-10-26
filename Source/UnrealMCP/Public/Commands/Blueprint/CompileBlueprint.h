#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

/**
 * Handles operations for compiling blueprints dynamically.
 */
class UNREALMCP_API FCompileBlueprint {
public:
	FCompileBlueprint() = default;

	~FCompileBlueprint() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};

}
