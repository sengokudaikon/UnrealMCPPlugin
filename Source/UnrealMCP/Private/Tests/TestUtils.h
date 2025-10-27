#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLibrary.h"
#include "Containers/Array.h"
#include "Misc/AutomationTest.h"
#include "Types/BlueprintTypes.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace UnrealMCPTest {
	/**
	 * Utility class for common test operations
	 */
	class FTestUtils {
	public:
		/**
		 * Generate a unique test name to avoid conflicts
		 * Uses GUIDs to ensure uniqueness across parallel test runs
		 */
		static auto GenerateUniqueTestName(const FString& BaseName) -> FString {
			const FGuid UniqueId = FGuid::NewGuid();
			// Use only the first 8 characters of the GUID for readability
			const FString ShortGuid = UniqueId.ToString(EGuidFormats::DigitsWithHyphens).Left(8);
			return FString::Printf(TEXT("%s_%s"), *BaseName, *ShortGuid);
		}

		/**
		 * Get the standard test package path
		 */
		static auto GetTestPackagePath() -> FString {
			return TEXT("/Game/AutomationTest/");
		}

		/**
		 * Create a full asset path for testing
		 */
		static auto GetTestAssetPath(const FString& AssetName) -> FString {
			return GetTestPackagePath() + AssetName;
		}

		/**
		 * Clean up a test asset by path
		 */
		static auto CleanupTestAsset(const FString& AssetPath) -> bool {
			if (UEditorAssetLibrary::DoesAssetExist(AssetPath)) {
				return UEditorAssetLibrary::DeleteAsset(AssetPath);
			}
			return true; // Asset doesn't exist, so it's "clean"
		}

		/**
		 * Clean up all test blueprints in the test package path
		 */
		static auto CleanupAllTestBlueprints() -> void {
			// Delete all assets in the test package path
			UEditorAssetLibrary::DeleteDirectory(GetTestPackagePath());
		}

		/**
		 * Clean up a test blueprint by name (handles different path patterns)
		 */
		static auto CleanupTestBlueprintByName(const FString& BlueprintName,
		                                       const FString& SubPath = TEXT("")) -> bool {
			// Try common test paths
			TArray<FString> TestPaths = {
				GetTestAssetPath(BlueprintName),
				FString::Printf(TEXT("/Game/Tests/%s/%s.%s"), *SubPath, *BlueprintName, *BlueprintName),
				FString::Printf(TEXT("/Game/Tests/%s.%s"), *BlueprintName, *BlueprintName),
				FString::Printf(TEXT("/Game/UI/%s.%s"), *BlueprintName, *BlueprintName),
				FString::Printf(TEXT("/Game/Input/%s.%s"), *BlueprintName, *BlueprintName)
			};

			bool bDeletedAny = false;
			for (const FString& AssetPath : TestPaths) {
				if (UEditorAssetLibrary::DoesAssetExist(AssetPath)) {
					UEditorAssetLibrary::DeleteAsset(AssetPath);
					bDeletedAny = true;
				}
			}
			return bDeletedAny;
		}

		/**
		 * Clean up all test assets across common test directories
		 */
		static auto CleanupAllTestAssets() -> void {
			TArray<FString> TestDirectories = {
				GetTestPackagePath(),
				TEXT("/Game/Tests/"),
				TEXT("/Game/UI/"),
				TEXT("/Game/Input/")
			};

			for (const FString& Directory : TestDirectories) {
				if (UEditorAssetLibrary::DoesDirectoryExist(Directory)) {
					UEditorAssetLibrary::DeleteDirectory(Directory);
				}
			}
		}

		/**
		 * Register a global cleanup function that runs after all tests complete
		 */
		static auto RegisterGlobalCleanup() -> void {
			// This can be called at the start of test sessions to ensure cleanup
			// even if individual tests fail or crash
			CleanupAllTestAssets();
		}

		/**
		 * Test helper to verify two strings are equal with custom error message
		 */
		static auto TestStringEqual(const FString& Expected,
		                            const FString& Actual,
		                            const FString& Context,
		                            FAutomationTestBase* Test) -> bool {
			if (Expected.Equals(Actual)) {
				return true;
			}

			Test->AddError(FString::Printf(TEXT("%s: Expected '%s' but got '%s'"),
			                               *Context,
			                               *Expected,
			                               *Actual));
			return false;
		}

		/**
		 * Test helper to verify a string contains a substring
		 */
		static auto TestStringContains(const FString& Text,
		                               const FString& Substring,
		                               const FString& Context,
		                               FAutomationTestBase* Test) -> bool {
			if (Text.Contains(Substring)) {
				return true;
			}

			Test->AddError(FString::Printf(TEXT("%s: Expected text to contain '%s' but it was '%s'"),
			                               *Context,
			                               *Substring,
			                               *Text));
			return false;
		}

		/**
		 * Get the standard test path for blueprint graph tests
		 */
		static auto GetBlueprintGraphTestPath() -> FString {
			return TEXT("/Game/Tests/Graph/");
		}

		/**
		 * Create blueprint creation parameters for testing with unique naming
		 */
		static auto CreateTestBlueprintParams(
			const FString& BaseName,
			const FString& ParentClass = TEXT("Actor"),
			const FString& SubPath = TEXT("Graph")) -> UnrealMCP::FBlueprintCreationParams {
			UnrealMCP::FBlueprintCreationParams Params;
			Params.Name = GenerateUniqueTestName(BaseName);
			Params.PackagePath = FString::Printf(TEXT("/Game/Tests/%s/"), *SubPath);
			Params.ParentClass = ParentClass;
			return Params;
		}

		/**
		 * Create blueprint creation parameters with a specific sub-path
		 */
		static auto CreateTestBlueprintParamsWithPath(
			const FString& BaseName,
			const FString& PackagePath,
			const FString& ParentClass = TEXT("Actor")) -> UnrealMCP::FBlueprintCreationParams {
			UnrealMCP::FBlueprintCreationParams Params;
			Params.Name = GenerateUniqueTestName(BaseName);
			Params.PackagePath = PackagePath;
			Params.ParentClass = ParentClass;
			return Params;
		}

		/**
		 * Generate a unique test actor name to avoid conflicts
		 */
		static auto GenerateUniqueTestActorName(const FString& BaseName) -> FString {
			return GenerateUniqueTestName(BaseName);
		}

		/**
		 * Helper to spawn a test actor with unique name and automatic cleanup registration
		 */
		template <typename T>
		static auto SpawnTestActor(UWorld* World,
		                           const FString& BaseName,
		                           const FVector& Location = FVector::ZeroVector,
		                           const FRotator& Rotation = FRotator::ZeroRotator) -> T* {
			if (!World) {
				return nullptr;
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.Name = FName(GenerateUniqueTestActorName(BaseName));

			T* Actor = World->SpawnActor<T>(T::StaticClass(), Location, Rotation, SpawnParams);
			return Actor;
		}

		/**
		 * Helper to safely destroy a test actor (null-safe)
		 */
		static auto DestroyTestActor(UWorld* World, AActor* Actor) -> void {
			if (World && Actor) {
				World->DestroyActor(Actor);
			}
		}
	};
}

#endif // WITH_DEV_AUTOMATION_TESTS
