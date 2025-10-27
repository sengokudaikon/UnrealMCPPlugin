#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnrealMCPModule : public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual auto StartupModule() -> void override;

	virtual auto ShutdownModule() -> void override;

	static inline auto Get() -> FUnrealMCPModule& {
		return FModuleManager::LoadModuleChecked<FUnrealMCPModule>("UnrealMCP");
	}

	static inline auto IsAvailable() -> bool {
		return FModuleManager::Get().IsModuleLoaded("UnrealMCP");
	}
};
