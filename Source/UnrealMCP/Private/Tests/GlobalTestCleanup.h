/**
 * Global test cleanup manager for UnrealMCP tests
 *
 * Provides centralized cleanup functionality that runs before and after test sessions
 * to ensure no test artifacts are left behind even if tests fail or crash.
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLibrary.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * Global test cleanup manager
 *
 * This class handles cleanup of test assets across the entire test suite.
 * It registers cleanup hooks that run before and after test execution.
 */
class FGlobalTestCleanupManager {
public:
	/**
	 * Get the singleton instance
	 */
	static auto Get() -> FGlobalTestCleanupManager& {
		static FGlobalTestCleanupManager Instance;
		return Instance;
	}

	/**
	 * Initialize the global cleanup manager
	 * Call this at the start of your test session
	 */
	auto Initialize() -> void {
		bIsInitialized = true;
		// Clean up any leftover assets from previous test runs
		CleanupAllTestAssets();
	}

	/**
	 * Clean up all test assets across all common test directories
	 */
	auto CleanupAllTestAssets() -> void {
		TArray<FString> TestDirectories = {
			TEXT("/Game/AutomationTest/"),
			TEXT("/Game/Tests/"),
			TEXT("/Game/UI/"),
			TEXT("/Game/Input/"),
			TEXT("/Game/Blueprints/") // For blueprint tests
		};

		for (const FString& Directory : TestDirectories) {
			if (UEditorAssetLibrary::DoesDirectoryExist(Directory)) {
				UE_LOG(LogTemp, Log, TEXT("Cleaning up test directory: %s"), *Directory);
				UEditorAssetLibrary::DeleteDirectory(Directory);
			}
		}
	}

	/**
	 * Clean up a specific test asset by name across all possible locations
	 */
	auto CleanupTestAsset(const FString& AssetName) -> bool {
		TArray<FString> PossiblePaths = {
			FString::Printf(TEXT("/Game/AutomationTest/%s.%s"), *AssetName, *AssetName),
			FString::Printf(TEXT("/Game/Tests/%s.%s"), *AssetName, *AssetName),
			FString::Printf(TEXT("/Game/UI/%s.%s"), *AssetName, *AssetName),
			FString::Printf(TEXT("/Game/Input/%s.%s"), *AssetName, *AssetName),
			FString::Printf(TEXT("/Game/Blueprints/%s.%s"), *AssetName, *AssetName)
		};

		bool bDeletedAny = false;
		for (const FString& AssetPath : PossiblePaths) {
			if (UEditorAssetLibrary::DoesAssetExist(AssetPath)) {
				UE_LOG(LogTemp, Log, TEXT("Cleaning up test asset: %s"), *AssetPath);
				UEditorAssetLibrary::DeleteAsset(AssetPath);
				bDeletedAny = true;
			}
		}
		return bDeletedAny;
	}

	/**
	 * Force cleanup all test assets (called when tests complete or crash)
	 */
	auto ForceCleanupAll() -> void {
		if (bIsInitialized) {
			CleanupAllTestAssets();
		}
	}

private:
	FGlobalTestCleanupManager() :
		bIsInitialized(false) {
	}

	~FGlobalTestCleanupManager() {
		// Destructor cleanup - runs when the application exits
		ForceCleanupAll();
	}

	bool bIsInitialized;

	// Prevent copying
	FGlobalTestCleanupManager(const FGlobalTestCleanupManager&) = delete;

	auto operator=(const FGlobalTestCleanupManager&) -> FGlobalTestCleanupManager& = delete;
};

/**
 * RAII helper class for automatic test cleanup
 *
 * Usage:
 *   // At the beginning of a test
 *   FTestCleanupGuard CleanupGuard;
 *
 *   // Test code here...
 *   // Cleanup happens automatically when the guard goes out of scope
 */
class FTestCleanupGuard {
public:
	FTestCleanupGuard() {
		// Constructor - can be used for pre-test cleanup if needed
	}

	~FTestCleanupGuard() {
		// Destructor - cleans up all test assets when the test completes
		FGlobalTestCleanupManager::Get().ForceCleanupAll();
	}
};

/**
 * Macro to easily add cleanup to the start of any test
 */
#define REGISTER_GLOBAL_CLEANUP() FGlobalTestCleanupManager::Get().Initialize()

/**
 * Macro to easily create a cleanup guard in tests
 */
#define CREATE_TEST_CLEANUP_GUARD() FTestCleanupGuard CleanupGuard

#endif // WITH_DEV_AUTOMATION_TESTS
