#include "Commands/UnrealMCPBlueprintNodeCommands.h"

#include "Commands/BlueprintNode/ConnectBlueprintNodes.h"
#include "Commands/BlueprintNode/AddBlueprintGetSelfComponentReference.h"
#include "Commands/BlueprintNode/AddBlueprintEvent.h"
#include "Commands/BlueprintNode/AddBlueprintFunctionCall.h"
#include "Commands/BlueprintNode/AddBlueprintVariable.h"
#include "Commands/BlueprintNode/AddBlueprintInputActionNode.h"
#include "Commands/BlueprintNode/AddBlueprintSelfReference.h"
#include "Commands/BlueprintNode/FindBlueprintNodes.h"
#include "Commands/CommonUtils.h"

FUnrealMCPBlueprintNodeCommands::FUnrealMCPBlueprintNodeCommands() {
	CommandHandlers.Add(TEXT("connect_blueprint_nodes"), &FConnectBlueprintNodes::Handle);
	CommandHandlers.Add(TEXT("add_blueprint_get_self_component_reference"), &FAddBlueprintGetSelfComponentReference::Handle);
	CommandHandlers.Add(TEXT("add_blueprint_self_reference"), &FAddBlueprintSelfReference::Handle);
	CommandHandlers.Add(TEXT("find_blueprint_nodes"), &FFindBlueprintNodes::Handle);
	CommandHandlers.Add(TEXT("add_blueprint_event_node"), &FAddBlueprintEvent::Handle);
	CommandHandlers.Add(TEXT("add_blueprint_input_action_node"), &FAddBlueprintInputActionNode::Handle);
	CommandHandlers.Add(TEXT("add_blueprint_function_node"), &FAddBlueprintFunctionCall::Handle);
	CommandHandlers.Add(TEXT("add_blueprint_variable"), &FAddBlueprintVariable::Handle);
}

TSharedPtr<FJsonObject> FUnrealMCPBlueprintNodeCommands::HandleCommand(
	const FString& CommandType,
	const TSharedPtr<FJsonObject>& Params
) {
	if (const auto* Handler = CommandHandlers.Find(CommandType)) {
		return (*Handler)(Params);
	}

	return FCommonUtils::CreateErrorResponse(
		FString::Printf(TEXT("Unknown blueprint node command: %s"), *CommandType));
}
