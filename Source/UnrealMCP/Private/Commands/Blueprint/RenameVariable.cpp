#include "Commands/Blueprint/RenameVariable.h"
#include "Core/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FRenameVariableCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("old_name")) ||
			!Params->HasField(TEXT("new_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name, old_name, and new_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString OldName = Params->GetStringField(TEXT("old_name"));
		const FString NewName = Params->GetStringField(TEXT("new_name"));

		// Validate new name
		if (NewName.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(TEXT("New variable name cannot be empty"));
		}

		// Find the blueprint
		UBlueprint* Blueprint =
			LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName));
		if (!Blueprint) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the old variable
		const FName OldVarFName = FName(*OldName);
		const int32 OldVarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, OldVarFName);

		if (OldVarIndex == INDEX_NONE) {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Variable '%s' not found in blueprint"), *OldName)
			);
		}

		// Check if new name already exists
		const FName NewVarFName = FName(*NewName);
		const int32 NewVarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, NewVarFName);

		if (NewVarIndex != INDEX_NONE) {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Variable with name '%s' already exists"), *NewName)
			);
		}

		// Rename the variable using Blueprint Editor Utils
		FBlueprintEditorUtils::RenameMemberVariable(Blueprint, OldVarFName, NewVarFName);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Variable renamed from '%s' to '%s'"), *OldName, *NewName)
			);
			Data->SetStringField(TEXT("old_name"), OldName);
			Data->SetStringField(TEXT("new_name"), NewName);
		});
	}

} // namespace UnrealMCP
