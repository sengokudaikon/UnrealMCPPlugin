#pragma once

#include "CoreMinimal.h"
#include "Json.h"

/**
 * Handler class for Blueprint Node-related MCP commands.
 *
 * This class acts as a command router, delegating to specialized
 * command handler classes for each blueprint node operation using a registry pattern.
 */
class UNREALMCP_API FUnrealMCPBlueprintNodeCommands {
public:
	FUnrealMCPBlueprintNodeCommands();

	/**
	 * Route a blueprint node command to the appropriate handler.
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
