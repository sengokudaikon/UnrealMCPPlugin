#include "Commands/Blueprint/RemoveFunction.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FRemoveFunctionCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name and function_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));
		
		if (const FVoidResult Result = FBlueprintMemberService::RemoveFunction(BlueprintName, FunctionName); Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Function '%s' removed from blueprint '%s'"), *FunctionName, *BlueprintName)
			);
		});
	}

}
