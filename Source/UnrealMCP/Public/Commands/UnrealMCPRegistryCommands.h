#pragma once

#include "CoreMinimal.h"
#include "Json.h"

/**
 * Handler class for Registry-related MCP commands.
 *
 * This class routes registry queries to their appropriate handlers.
 * Registry commands allow querying what's available and supported in the API.
 */
class UNREALMCP_API FUnrealMCPRegistryCommands {
public:
	FUnrealMCPRegistryCommands();

	/**
	 * Route a registry command to the appropriate handler.
	 *
	 * @param CommandType The type of command to execute
	 * @param Params JSON parameters for the command
	 * @return JSON response object
	 */
	TSharedPtr<FJsonObject> HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params);

private:
	/** Type definition for command handler function pointers */
	using FCommandHandler = TSharedPtr<FJsonObject> (*)(const TSharedPtr<FJsonObject>&);

	/** Registry mapping command types to their handler functions */
	TMap<FString, FCommandHandler> CommandHandlers;
};
