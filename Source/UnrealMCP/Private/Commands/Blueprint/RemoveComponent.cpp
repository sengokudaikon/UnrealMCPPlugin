#include "Commands/Blueprint/RemoveComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Services/BlueprintService.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	auto FRemoveComponentCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("component_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing required parameters: blueprint_name and component_name"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString ComponentName = Params->GetStringField(TEXT("component_name"));

		// Find the blueprint
		UBlueprint* Blueprint =
			LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName));
		if (!Blueprint) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the component
		USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return FCommonUtils::CreateErrorResponse(TEXT("Blueprint has no construction script"));
		}

		USCS_Node* NodeToRemove = nullptr;
		for (USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == ComponentName) {
				NodeToRemove = Node;
				break;
			}
		}

		if (!NodeToRemove) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Component '%s' not found in blueprint"), *ComponentName));
		}

		// Remove the node
		SCS->RemoveNode(NodeToRemove);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Component '%s' removed from blueprint '%s'"), *ComponentName, *BlueprintName)
			);
		});
	}

} // namespace UnrealMCP
