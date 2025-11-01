#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLibrary.h"
#include "Containers/Array.h"
#include "Misc/AutomationTest.h"
#include "Types/BlueprintTypes.h"
#include "Core/ErrorTypes.h"
#include "Core/Result.h"

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

		/**
		 * Validate error contains the expected error code and optionally checks context
		 * This is the preferred way to validate errors - check error codes, not message text!
		 */
		static auto ValidateErrorCode(const UnrealMCP::FVoidResult& Result,
		                             UnrealMCP::EErrorCode ExpectedErrorCode,
		                             const FString& ExpectedContext,
		                             FAutomationTestBase* Test) -> bool {
			if (Result.IsSuccess()) {
				Test->AddError(FString::Printf(
					TEXT("Expected error code %d but operation succeeded"),
					static_cast<int32>(ExpectedErrorCode)));
				return false;
			}

			UnrealMCP::EErrorCode ActualErrorCode = Result.GetErrorCode();
			if (ActualErrorCode != ExpectedErrorCode) {
				Test->AddError(FString::Printf(
					TEXT("Expected error code %d (%s) but got %d (%s)"),
					static_cast<int32>(ExpectedErrorCode),
					*UnrealMCP::FError::GetErrorCodeName(ExpectedErrorCode),
					static_cast<int32>(ActualErrorCode),
					*UnrealMCP::FError::GetErrorCodeName(ActualErrorCode)));
				return false;
			}

			// Optionally check context if provided
			if (!ExpectedContext.IsEmpty()) {
				const FString& ActualContext = Result.GetError().Context;
				if (!ActualContext.Contains(ExpectedContext)) {
					Test->AddError(FString::Printf(
						TEXT("Expected error context '%s' but got '%s'"),
						*ExpectedContext,
						*ActualContext));
					return false;
				}
			}

			return true;
		}

		/**
		 * Validate error contains the expected error code for templated results
		 */
		template <typename T>
		static auto ValidateErrorCode(const UnrealMCP::TResult<T>& Result,
		                             UnrealMCP::EErrorCode ExpectedErrorCode,
		                             const FString& ExpectedContext,
		                             FAutomationTestBase* Test) -> bool {
			if (Result.IsSuccess()) {
				Test->AddError(FString::Printf(
					TEXT("Expected error code %d but operation succeeded"),
					static_cast<int32>(ExpectedErrorCode)));
				return false;
			}

			UnrealMCP::EErrorCode ActualErrorCode = Result.GetErrorCode();
			if (ActualErrorCode != ExpectedErrorCode) {
				Test->AddError(FString::Printf(
					TEXT("Expected error code %d (%s) but got %d (%s)"),
					static_cast<int32>(ExpectedErrorCode),
					*UnrealMCP::FError::GetErrorCodeName(ExpectedErrorCode),
					static_cast<int32>(ActualErrorCode),
					*UnrealMCP::FError::GetErrorCodeName(ActualErrorCode)));
				return false;
			}

			// Optionally check context if provided
			if (!ExpectedContext.IsEmpty()) {
				const FString& ActualContext = Result.GetError().Context;
				if (!ActualContext.Contains(ExpectedContext)) {
					Test->AddError(FString::Printf(
						TEXT("Expected error context '%s' but got '%s'"),
						*ExpectedContext,
						*ActualContext));
					return false;
				}
			}

			return true;
		}

		/**
		 * Legacy: Validate error message contains expected context (for backward compatibility)
		 * DEPRECATED: Use ValidateErrorCode instead!
		 */
		static auto ValidateErrorMessage(const FString& ErrorMessage,
										  const FString& OperationType,   // "spawn", "add", "set", etc.
										  const FString& ResourceType,    // "Blueprint", "Actor", "Component", etc.
										  const FString& ResourceName,    // The specific item name
										  FAutomationTestBase* Test) -> bool {
			Test->AddWarning(TEXT("ValidateErrorMessage is deprecated. Use ValidateErrorCode instead."));

			bool bValid = true;

			// Check operation is mentioned
			if (!ErrorMessage.Contains(OperationType)) {
				Test->AddError(FString::Printf(
					TEXT("Error missing operation context. Expected '%s' in: %s"),
					*OperationType,
					*ErrorMessage));
				bValid = false;
			}

			// Check resource type is mentioned
			if (!ErrorMessage.Contains(ResourceType)) {
				Test->AddError(FString::Printf(
					TEXT("Error missing resource type. Expected '%s' in: %s"),
					*ResourceType,
					*ErrorMessage));
				bValid = false;
			}

			// Check specific resource is mentioned
			if (!ErrorMessage.Contains(ResourceName)) {
				Test->AddError(FString::Printf(
					TEXT("Error missing resource name. Expected '%s' in: %s"),
					*ResourceName,
					*ErrorMessage));
				bValid = false;
			}

			return bValid;
		}

		/**
		 * Validate that two different resource names produce different error messages
		 * (ensures errors are specific, not generic)
		 */
		static auto ValidateErrorsAreDifferent(const FString& Error1,
											   const FString& Error2,
											   const FString& ResourceName1,
											   const FString& ResourceName2,
											   FAutomationTestBase* Test) -> bool {
			if (Error1 == Error2) {
				Test->AddError(FString::Printf(
					TEXT("Error messages should differ for different resources. Both: %s"),
					*Error1));
				return false;
			}

			// Verify first error mentions the correct resource
			if (!Error1.Contains(ResourceName1)) {
				Test->AddError(FString::Printf(
					TEXT("First error should mention '%s', got: %s"),
					*ResourceName1,
					*Error1));
				return false;
			}

			// Verify second error mentions the correct resource
			if (!Error2.Contains(ResourceName2)) {
				Test->AddError(FString::Printf(
					TEXT("Second error should mention '%s', got: %s"),
					*ResourceName2,
					*Error2));
				return false;
			}

			return true;
		}

		/**
		 * Validate error distinguishes between different failure types
		 * For example: missing blueprint vs. invalid component type
		 */
		static auto ValidateErrorTypeDistinction(const FString& Error,
												 const FString& ExpectedFailureReason,
												 const FString& UnexpectedReason,
												 FAutomationTestBase* Test) -> bool {
			if (!Error.Contains(ExpectedFailureReason)) {
				Test->AddError(FString::Printf(
					TEXT("Error should mention '%s', got: %s"),
					*ExpectedFailureReason,
					*Error));
				return false;
			}

			// Optional: check it doesn't confuse this with a different error type
			if (!UnexpectedReason.IsEmpty() && Error.Contains(UnexpectedReason)) {
				Test->AddWarning(FString::Printf(
					TEXT("Error mentions both '%s' and '%s' - may be confusing"),
					*ExpectedFailureReason,
					*UnexpectedReason));
			}

			return true;
		}
	};
}

#endif // WITH_DEV_AUTOMATION_TESTS
