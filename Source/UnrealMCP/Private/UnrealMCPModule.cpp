#include "UnrealMCPModule.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FUnrealMCPModule"

auto FUnrealMCPModule::StartupModule() -> void {
	UE_LOG(LogTemp, Display, TEXT("Unreal MCP Module has started"));
}

auto FUnrealMCPModule::ShutdownModule() -> void {
	UE_LOG(LogTemp, Display, TEXT("Unreal MCP Module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealMCPModule, UnrealMCP)
