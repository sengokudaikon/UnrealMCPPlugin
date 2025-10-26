#pragma once

#include "CoreMinimal.h"
#include "Json.h"

namespace UnrealMCP {

	class UNREALMCP_API FBlueprintExistsCommand {
	public:
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

	class UNREALMCP_API FGetBlueprintInfoCommand {
	public:
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

	class UNREALMCP_API FGetBlueprintComponentsCommand {
	public:
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

	class UNREALMCP_API FGetBlueprintVariablesCommand {
	public:
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

	class UNREALMCP_API FGetBlueprintPathCommand {
	public:
		static auto Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};

} // namespace UnrealMCP
