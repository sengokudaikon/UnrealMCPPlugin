#include "Commands/Blueprint/RenameVariable.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FRenameVariableCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("old_name")) ||
			!Params->HasField(TEXT("new_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name, old_name, and new_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString OldName = Params->GetStringField(TEXT("old_name"));
		const FString NewName = Params->GetStringField(TEXT("new_name"));


		if (const FVoidResult Result = FBlueprintMemberService::RenameVariable(BlueprintName, OldName, NewName); Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Variable renamed from '%s' to '%s'"), *OldName, *NewName)
			);
			Data->SetStringField(TEXT("old_name"), OldName);
			Data->SetStringField(TEXT("new_name"), NewName);
		});
	}

}
