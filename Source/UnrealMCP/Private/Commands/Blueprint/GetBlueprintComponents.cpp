#include "Commands/Blueprint/GetBlueprintComponents.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FGetBlueprintComponentsCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		TArray<TMap<FString, FString>> Components;

		if (const FVoidResult Result = FBlueprintIntrospectionService::GetBlueprintComponents(BlueprintName, Components)
			; !Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			TArray<TSharedPtr<FJsonValue>> JsonArray;
			for (const TMap<FString, FString>& ComponentInfo : Components) {
				TSharedPtr<FJsonObject> ComponentObject = MakeShared<FJsonObject>();
				for (const auto& Pair : ComponentInfo) {
					ComponentObject->SetStringField(Pair.Key, Pair.Value);
				}
				JsonArray.Add(MakeShared<FJsonValueObject>(ComponentObject));
			}
			Data->SetArrayField(TEXT("components"), JsonArray);
			Data->SetNumberField(TEXT("count"), Components.Num());
		});
	}

}
