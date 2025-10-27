#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "HAL/Runnable.h"
#include "Interfaces/IPv4/IPv4Address.h"

class UUnrealMCPBridge;

/**
 * Runnable class for the MCP server thread
 */
class FMCPServerRunnable : public FRunnable {
public:
	FMCPServerRunnable(UUnrealMCPBridge* InBridge, TSharedPtr<FSocket> InListenerSocket);

	virtual ~FMCPServerRunnable() override;

	// FRunnable interface
	virtual auto Init() -> bool override;

	virtual auto Run() -> uint32 override;

	virtual auto Stop() -> void override;

	virtual auto Exit() -> void override;

protected:
	auto HandleClientConnection(TSharedPtr<FSocket> ClientSocket) -> void;

	auto ProcessMessage(TSharedPtr<FSocket> Client, const FString& Message) const -> void;

private:
	UUnrealMCPBridge* Bridge;
	TSharedPtr<FSocket> ListenerSocket;
	TSharedPtr<FSocket> ClientSocket;
	bool bRunning;
};
