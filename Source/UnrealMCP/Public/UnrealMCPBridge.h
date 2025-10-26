#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Sockets.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Commands/UnrealMCPEditorCommands.h"
#include "Commands/UnrealMCPBlueprintCommands.h"
#include "Commands/UnrealMCPBlueprintNodeCommands.h"
#include "Commands/UnrealMCPInputCommands.h"
#include "Commands/UnrealMCPWidgetCommands.h"
#include "Commands/UnrealMCPRegistryCommands.h"
#include "UnrealMCPBridge.generated.h"

class FMCPServerRunnable;

/**
 * Editor subsystem for MCP Bridge
 * Handles communication between external tools and the Unreal Editor
 * through a TCP socket connection. Commands are received as JSON and
 * routed to appropriate command handlers.
 */
UCLASS()
class UNREALMCP_API UUnrealMCPBridge : public UEditorSubsystem {
	GENERATED_BODY()

public:
	UUnrealMCPBridge();

	virtual ~UUnrealMCPBridge() override;

	// UEditorSubsystem implementation
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	// Server functions
	void StartServer();

	void StopServer();

	bool IsRunning() const {
		return bIsRunning;
	}

	// Command execution
	FString ExecuteCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params);

private:
	// Server state
	bool bIsRunning;
	TSharedPtr<FSocket> ListenerSocket;
	TSharedPtr<FSocket> ConnectionSocket;
	FRunnableThread* ServerThread;

	// Server configuration
	FIPv4Address ServerAddress;
	uint16 Port;

	// Command handler instances
	TSharedPtr<UnrealMCP::FUnrealMCPEditorCommands> EditorCommands;
	TSharedPtr<UnrealMCP::FUnrealMCPBlueprintCommands> BlueprintCommands;
	TSharedPtr<UnrealMCP::FUnrealMCPBlueprintNodeCommands> BlueprintNodeCommands;
	TSharedPtr<UnrealMCP::FUnrealMCPInputCommands> InputCommands;
	TSharedPtr<UnrealMCP::FUnrealMCPWidgetCommands> UMGCommands;
	TSharedPtr<UnrealMCP::FUnrealMCPRegistryCommands> RegistryCommands;

	enum class ECommandHandlerType { Editor, Blueprint, BlueprintNode, Input, Widget, Registry, Ping };

	TMap<FString, ECommandHandlerType> CommandRoutingMap;

	void InitializeCommandRouting();
};
