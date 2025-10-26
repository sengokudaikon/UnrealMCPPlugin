#include "Commands/Registry/GetSupportedComponentTypes.h"
#include "Core/MCPRegistry.h"
#include "Commands/CommonUtils.h"

namespace UnrealMCP {

	TSharedPtr<FJsonObject> FGetSupportedComponentTypesCommand::Execute(const TSharedPtr<FJsonObject>& Params) {
		TArray<FString> ComponentTypes;
		const FVoidResult Result = FMCPRegistry::GetSupportedComponentTypes(ComponentTypes);

		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		TArray<TSharedPtr<FJsonValue>> JsonArray;
		for (const FString& ComponentType : ComponentTypes) {
			JsonArray.Add(MakeShared<FJsonValueString>(ComponentType));
		}

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetArrayField(TEXT("component_types"), JsonArray);
		Response->SetNumberField(TEXT("count"), ComponentTypes.Num());

		return Response;
	}

} // namespace UnrealMCP
