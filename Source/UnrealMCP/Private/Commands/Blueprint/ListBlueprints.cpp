#include "Commands/Blueprint/ListBlueprints.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Commands/CommonUtils.h"

namespace UnrealMCP {

	auto FListBlueprintsCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		const FString Path = Params->HasField(TEXT("path")) ? Params->GetStringField(TEXT("path")) : TEXT("/Game/");
		const bool bRecursive = Params->HasField(TEXT("recursive")) ? Params->GetBoolField(TEXT("recursive")) : true;

		TArray<FString> Blueprints;
		const FVoidResult Result = FBlueprintIntrospectionService::ListBlueprints(Path, bRecursive, Blueprints);

		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		TArray<TSharedPtr<FJsonValue>> JsonArray;
		for (const FString& BlueprintPath : Blueprints) {
			JsonArray.Add(MakeShared<FJsonValueString>(BlueprintPath));
		}

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetArrayField(TEXT("blueprints"), JsonArray);
		Response->SetNumberField(TEXT("count"), Blueprints.Num());

		return Response;
	}

} // namespace UnrealMCP
