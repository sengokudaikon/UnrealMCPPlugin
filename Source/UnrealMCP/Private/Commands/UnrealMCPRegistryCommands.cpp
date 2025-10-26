#include "Commands/UnrealMCPRegistryCommands.h"
#include "Commands/Registry/GetSupportedParentClasses.h"
#include "Commands/Registry/GetSupportedComponentTypes.h"
#include "Commands/Registry/GetAvailableAPIMethods.h"
#include "Commands/CommonUtils.h"

using namespace UnrealMCP;

FUnrealMCPRegistryCommands::FUnrealMCPRegistryCommands() {
	// Register command handlers
	CommandHandlers.Add(TEXT("get_supported_parent_classes"), &FGetSupportedParentClassesCommand::Execute);
	CommandHandlers.Add(TEXT("get_supported_component_types"), &FGetSupportedComponentTypesCommand::Execute);
	CommandHandlers.Add(TEXT("get_available_api_methods"), &FGetAvailableAPIMethodsCommand::Execute);
}

TSharedPtr<FJsonObject> FUnrealMCPRegistryCommands::HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params) {
	// Look up command handler
	const FCommandHandler* Handler = CommandHandlers.Find(CommandType);
	if (!Handler) {
		const FString ErrorMsg = FString::Printf(TEXT("Unknown registry command: %s"), *CommandType);
		return FCommonUtils::CreateErrorResponse(ErrorMsg);
	}

	// Execute command handler
	return (*Handler)(Params);
}
