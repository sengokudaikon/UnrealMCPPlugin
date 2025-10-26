#include "Commands/Blueprint/RenameComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	auto FRenameComponentCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("old_name")) ||
			!Params->HasField(TEXT("new_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing required parameters: blueprint_name, old_name, and new_name"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString OldName = Params->GetStringField(TEXT("old_name"));
		const FString NewName = Params->GetStringField(TEXT("new_name"));

		// Validate new name
		if (NewName.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(TEXT("New component name cannot be empty"));
		}

		// Find the blueprint
		UBlueprint* Blueprint =
			LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName));
		if (!Blueprint) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the component
		const USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return FCommonUtils::CreateErrorResponse(TEXT("Blueprint has no construction script"));
		}

		USCS_Node* TargetNode = nullptr;
		for (USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == OldName) {
				TargetNode = Node;
				break;
			}
		}

		if (!TargetNode) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Component '%s' not found in blueprint"), *OldName));
		}

		// Check if new name already exists
		for (const USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == NewName) {
				return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Component with name '%s' already exists"), *NewName));
			}
		}

		// Rename the component
		const auto NewFName = FName(*NewName);
		FBlueprintEditorUtils::RenameComponentMemberVariable(Blueprint, TargetNode, NewFName);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"), FString::Printf(TEXT("Component renamed from '%s' to '%s'"), *OldName, *NewName)
			);
			Data->SetStringField(TEXT("old_name"), OldName);
			Data->SetStringField(TEXT("new_name"), NewName);
		});
	}

} // namespace UnrealMCP
