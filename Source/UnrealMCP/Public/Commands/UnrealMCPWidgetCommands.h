#pragma once

#include "CoreMinimal.h"
#include "Json.h"

/**
 * Handles UMG (Widget Blueprint) related MCP commands
 * Responsible for creating and modifying UMG Widget Blueprints,
 * adding widget components, and managing widget instances in the viewport.
 */
class UNREALMCP_API FUnrealMCPWidgetCommands
{
public:
    FUnrealMCPWidgetCommands();

    /**
     * Handle UMG-related commands
     * @param CommandType - The type of command to handle
     * @param Params - JSON parameters for the command
     * @return JSON response with results or error
     */
    TSharedPtr<FJsonObject> HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params);

private:
    // Map of command name to handler function
    TMap<FString, TSharedPtr<FJsonObject>(*)(const TSharedPtr<FJsonObject>&)> CommandHandlers;
}; 