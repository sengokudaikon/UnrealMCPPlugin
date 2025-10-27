#include "Commands/Blueprint/GetBlueprintVariables.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FGetBlueprintVariablesCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));

		auto Result = FBlueprintIntrospectionService::GetBlueprintVariables(BlueprintName);
		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		const FGetBlueprintVariablesResult& VariablesResult = Result.GetValue();
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetObjectField(TEXT("result"), VariablesResult.ToJson());
		});
	}

}
