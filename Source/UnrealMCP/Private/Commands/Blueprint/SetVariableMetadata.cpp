#include "Commands/Blueprint/SetVariableMetadata.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FSetVariableMetadataCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("variable_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name and variable_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString VariableName = Params->GetStringField(TEXT("variable_name"));

		TOptional<FString> Tooltip;
		if (Params->HasField(TEXT("tooltip"))) {
			Tooltip = Params->GetStringField(TEXT("tooltip"));
		}

		TOptional<FString> Category;
		if (Params->HasField(TEXT("category"))) {
			Category = Params->GetStringField(TEXT("category"));
		}

		TOptional<bool> bExposeOnSpawn;
		if (Params->HasField(TEXT("expose_on_spawn"))) {
			bExposeOnSpawn = Params->GetBoolField(TEXT("expose_on_spawn"));
		}

		TOptional<bool> bInstanceEditable;
		if (Params->HasField(TEXT("instance_editable"))) {
			bInstanceEditable = Params->GetBoolField(TEXT("instance_editable"));
		}

		TOptional<bool> bBlueprintReadOnly;
		if (Params->HasField(TEXT("blueprint_read_only"))) {
			bBlueprintReadOnly = Params->GetBoolField(TEXT("blueprint_read_only"));
		}

		const FVoidResult Result = FBlueprintMemberService::SetVariableMetadata(
			BlueprintName,
			VariableName,
			Tooltip,
			Category,
			bExposeOnSpawn,
			bInstanceEditable,
			bBlueprintReadOnly
		);


		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Metadata set for variable '%s' in blueprint '%s'"), *VariableName, *BlueprintName)
			);
			Data->SetStringField(TEXT("variable_name"), VariableName);
		});
	}

}
