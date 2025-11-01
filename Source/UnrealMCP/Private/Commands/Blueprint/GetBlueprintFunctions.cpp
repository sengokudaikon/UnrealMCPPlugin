#include "Commands/Blueprint/GetBlueprintFunctions.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FGetBlueprintFunctionsCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing required parameter: blueprint_name")));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));

		auto Result = FBlueprintMemberService::GetFunctions(BlueprintName);
		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		const FGetBlueprintFunctionsResult& FunctionsResult = Result.GetValue();
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetObjectField(TEXT("result"), FunctionsResult.ToJson());
		});
	}

}
