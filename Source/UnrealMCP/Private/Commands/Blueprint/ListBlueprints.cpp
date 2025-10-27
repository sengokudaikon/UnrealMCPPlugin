#include "Commands/Blueprint/ListBlueprints.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FListBlueprintsCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		const FString Path = Params->HasField(TEXT("path")) ? Params->GetStringField(TEXT("path")) : TEXT("/Game/");
		const bool bRecursive = Params->HasField(TEXT("recursive")) ? Params->GetBoolField(TEXT("recursive")) : true;

		TArray<FString> Blueprints;

		if (const FVoidResult Result = FBlueprintIntrospectionService::ListBlueprints(Path, bRecursive, Blueprints); !
			Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			TArray<TSharedPtr<FJsonValue>> JsonArray;
			for (const FString& BlueprintPath : Blueprints) {
				JsonArray.Add(MakeShared<FJsonValueString>(BlueprintPath));
			}
			Data->SetArrayField(TEXT("blueprints"), JsonArray);
			Data->SetNumberField(TEXT("count"), Blueprints.Num());
		});
	}

}
