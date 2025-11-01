#include "Commands/UnrealMCPRegistryCommands.h"
#include "Commands/Registry/GetAvailableAPIMethods.h"
#include "Commands/Registry/GetSupportedComponentTypes.h"
#include "Commands/Registry/GetSupportedParentClasses.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"

namespace UnrealMCP {

	FUnrealMCPRegistryCommands::FUnrealMCPRegistryCommands() {
		// Register command handlers
		CommandHandlers.Add(TEXT("get_supported_parent_classes"), &FGetSupportedParentClassesCommand::Handle);
		CommandHandlers.Add(TEXT("get_supported_component_types"), &FGetSupportedComponentTypesCommand::Handle);
		CommandHandlers.Add(TEXT("get_available_api_methods"), &FGetAvailableAPIMethodsCommand::Handle);
	}

	auto FUnrealMCPRegistryCommands::HandleCommand(const FString& CommandType,
	                                               const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Look up command handler
		const FCommandHandler* Handler = CommandHandlers.Find(CommandType);
		if (!Handler) {
			const FString ErrorMsg = FString::Printf(TEXT("Unknown registry command: %s"), *CommandType);
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::OperationFailed, ErrorMsg));
		}

		// Execute command handler
		return (*Handler)(Params);
	}
}
