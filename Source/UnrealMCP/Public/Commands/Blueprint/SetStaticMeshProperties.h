#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

class UNREALMCP_API FSetStaticMeshProperties {
public:
	FSetStaticMeshProperties() = default;

	~FSetStaticMeshProperties() = default;

	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
}
