#include "Commands/Blueprint/RemoveVariable.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FRemoveVariableCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("variable_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name and variable_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString VariableName = Params->GetStringField(TEXT("variable_name"));


		if (const FVoidResult Result = FBlueprintMemberService::RemoveVariable(BlueprintName, VariableName); Result.
			IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Variable '%s' removed from blueprint '%s'"), *VariableName, *BlueprintName)
			);
		});
	}

}
