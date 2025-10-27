#include "Commands/Blueprint/AddFunction.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FAddFunctionCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name and function_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));

		if (const TResult<FString> Result = FBlueprintMemberService::AddFunction(BlueprintName, FunctionName); Result.
			IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Function '%s' added to blueprint '%s'"), *FunctionName, *BlueprintName)
			);
			Data->SetStringField(TEXT("function_name"), FunctionName);
		});
	}
}
