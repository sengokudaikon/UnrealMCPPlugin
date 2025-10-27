#include "Commands/Registry/GetSupportedComponentTypes.h"
#include "Core/CommonUtils.h"
#include "Core/MCPRegistry.h"

namespace UnrealMCP {

	auto FGetSupportedComponentTypesCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		TArray<FString> ComponentTypes;

		if (const FVoidResult Result = FMCPRegistry::GetSupportedComponentTypes(ComponentTypes); !Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			TArray<TSharedPtr<FJsonValue>> JsonArray;
			for (const FString& ComponentType : ComponentTypes) {
				JsonArray.Add(MakeShared<FJsonValueString>(ComponentType));
			}
			Data->SetArrayField(TEXT("component_types"), JsonArray);
			Data->SetNumberField(TEXT("count"), ComponentTypes.Num());
		});
	}

}
