/**
 * Functional tests for InputService
 *
 * These tests verify the actual behavior of Enhanced Input System operations:
 * - Creating input actions
 * - Creating input mapping contexts
 * - Adding/removing mappings
 * - Applying mapping contexts at runtime
 * - Creating legacy input mappings
 *
 * Tests run in the Unreal Editor with real world context.
 */

#include "Services/InputService.h"
#include "Core/MCPTypes.h"
#include "Misc/AutomationTest.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Editor.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateInputActionTest,
	"UnrealMCP.Input.CreateInputAction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateInputActionTest::RunTest(const FString& Parameters)
{
	// Test: Create a basic boolean input action

	UnrealMCP::FInputActionParams Params;
	Params.Name = TEXT("Jump");
	Params.ValueType = TEXT("Boolean");
	Params.Path = TEXT("/Game/Input");

	UnrealMCP::TResult<UInputAction*> Result = UnrealMCP::FInputService::CreateInputAction(Params);

	// Verify success
	TestTrue(TEXT("CreateInputAction should succeed"), Result.IsSuccess());
	UInputAction* InputAction = Result.GetValue();
	TestTrue(TEXT("InputAction should not be null"), InputAction != nullptr);
	if (InputAction)
	{
		TestEqual(TEXT("Action name should be prefixed with IA_"), InputAction->GetFName().ToString(), TEXT("IA_Jump"));
		TestEqual(TEXT("Value type should be Boolean"), static_cast<uint8>(InputAction->ValueType), static_cast<uint8>(EInputActionValueType::Boolean));
	}

	// Cleanup - delete the created asset
	if (InputAction)
	{
		const UPackage* Package = InputAction->GetOutermost();
		if (Package)
		{
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			const FString PackagePath = Package->GetPathName();
			const FString FilePath = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

			// Try to delete the file
			PlatformFile.DeleteFile(*FilePath);
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateInvalidInputActionTest,
	"UnrealMCP.Input.CreateInvalidInputAction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateInvalidInputActionTest::RunTest(const FString& Parameters)
{
	// Test: Creating input action with empty name should fail

	UnrealMCP::FInputActionParams Params;
	Params.Name = TEXT("");  // Empty name
	Params.ValueType = TEXT("Boolean");
	Params.Path = TEXT("/Game/Input");

	const UnrealMCP::TResult<UInputAction*> Result = UnrealMCP::FInputService::CreateInputAction(Params);

	// Verify failure
	TestTrue(TEXT("CreateInputAction should fail with empty name"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention name cannot be empty"),
		Result.GetError().Contains(TEXT("cannot be empty")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateInputMappingContextTest,
	"UnrealMCP.Input.CreateInputMappingContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateInputMappingContextTest::RunTest(const FString& Parameters)
{
	// Test: Create an input mapping context

	UnrealMCP::FInputMappingContextParams Params;
	Params.Name = TEXT("Gameplay");
	Params.Path = TEXT("/Game/Input");

	UnrealMCP::TResult<UInputMappingContext*> Result = UnrealMCP::FInputService::CreateInputMappingContext(Params);

	// Verify success
	TestTrue(TEXT("CreateInputMappingContext should succeed"), Result.IsSuccess());
	const UInputMappingContext* MappingContext = Result.GetValue();
	TestTrue(TEXT("MappingContext should not be null"), MappingContext != nullptr);
	if (MappingContext)
	{
		TestEqual(TEXT("Context name should be prefixed with IMC_"), MappingContext->GetFName().ToString(), TEXT("IMC_Gameplay"));
	}

	// Cleanup - delete the created asset
	if (MappingContext)
	{
		const UPackage* Package = MappingContext->GetOutermost();
		if (Package)
		{
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			const FString PackagePath = Package->GetPathName();
			const FString FilePath = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

			// Try to delete the file
			PlatformFile.DeleteFile(*FilePath);
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateInvalidMappingContextTest,
	"UnrealMCP.Input.CreateInvalidMappingContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateInvalidMappingContextTest::RunTest(const FString& Parameters)
{
	// Test: Creating mapping context with empty name should fail

	UnrealMCP::FInputMappingContextParams Params;
	Params.Name = TEXT("");  // Empty name
	Params.Path = TEXT("/Game/Input");

	const UnrealMCP::TResult<UInputMappingContext*> Result = UnrealMCP::FInputService::CreateInputMappingContext(Params);

	// Verify failure
	TestTrue(TEXT("CreateInputMappingContext should fail with empty name"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention name cannot be empty"),
		Result.GetError().Contains(TEXT("cannot be empty")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceAddMappingToInvalidContextTest,
	"UnrealMCP.Input.AddMappingToInvalidContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceAddMappingToInvalidContextTest::RunTest(const FString& Parameters)
{
	// Test: Adding mapping to non-existent context should fail

	UnrealMCP::FAddMappingParams Params;
	Params.ContextPath = TEXT("/Game/Input/NonExistentContext_XYZ123");
	Params.ActionPath = TEXT("/Game/Input/NonExistentAction_XYZ123");
	Params.Key = TEXT("Space");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::AddMappingToContext(Params);

	// Verify failure
	TestTrue(TEXT("AddMappingToContext should fail for non-existent context"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention failed to load"),
		Result.GetError().Contains(TEXT("Failed to load")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceAddMappingWithEmptyParametersTest,
	"UnrealMCP.Input.AddMappingWithEmptyParameters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceAddMappingWithEmptyParametersTest::RunTest(const FString& Parameters)
{
	// Test: Adding mapping with empty context path should fail

	UnrealMCP::FAddMappingParams Params;
	Params.ContextPath = TEXT("");  // Empty context path
	Params.ActionPath = TEXT("/Game/Input/TestAction");
	Params.Key = TEXT("Space");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::AddMappingToContext(Params);

	// Verify failure
	TestTrue(TEXT("AddMappingToContext should fail with empty context path"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention path cannot be empty"),
		Result.GetError().Contains(TEXT("cannot be empty")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceRemoveMappingFromInvalidContextTest,
	"UnrealMCP.Input.RemoveMappingFromInvalidContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceRemoveMappingFromInvalidContextTest::RunTest(const FString& Parameters)
{
	// Test: Removing mapping from non-existent context should fail

	UnrealMCP::FAddMappingParams Params;
	Params.ContextPath = TEXT("/Game/Input/NonExistentContext_XYZ123");
	Params.ActionPath = TEXT("/Game/Input/NonExistentAction_XYZ123");
	Params.Key = TEXT("Space");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::RemoveMappingFromContext(Params);

	// Verify failure
	TestTrue(TEXT("RemoveMappingFromContext should fail for non-existent context"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention failed to load"),
		Result.GetError().Contains(TEXT("Failed to load")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceApplyInvalidMappingContextTest,
	"UnrealMCP.Input.ApplyInvalidMappingContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceApplyInvalidMappingContextTest::RunTest(const FString& Parameters)
{
	// Test: Applying non-existent mapping context should fail

	UnrealMCP::FApplyMappingContextParams Params;
	Params.ContextPath = TEXT("/Game/Input/NonExistentContext_XYZ123");
	Params.Priority = 1;

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::ApplyMappingContext(Params);

	// Verify failure
	TestTrue(TEXT("ApplyMappingContext should fail for non-existent context"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention failed to load"),
		Result.GetError().Contains(TEXT("Failed to load")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceApplyMappingContextWithEmptyPathTest,
	"UnrealMCP.Input.ApplyMappingContextWithEmptyPath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceApplyMappingContextWithEmptyPathTest::RunTest(const FString& Parameters)
{
	// Test: Applying mapping context with empty path should fail

	UnrealMCP::FApplyMappingContextParams Params;
	Params.ContextPath = TEXT("");  // Empty path
	Params.Priority = 1;

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::ApplyMappingContext(Params);

	// Verify failure
	TestTrue(TEXT("ApplyMappingContext should fail with empty path"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention path cannot be empty"),
		Result.GetError().Contains(TEXT("cannot be empty")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceRemoveInvalidMappingContextTest,
	"UnrealMCP.Input.RemoveInvalidMappingContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceRemoveInvalidMappingContextTest::RunTest(const FString& Parameters)
{
	// Test: Removing non-existent mapping context should fail

	UnrealMCP::FRemoveMappingContextParams Params;
	Params.ContextPath = TEXT("/Game/Input/NonExistentContext_XYZ123");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::RemoveMappingContext(Params);

	// Verify failure
	TestTrue(TEXT("RemoveMappingContext should fail for non-existent context"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention failed to load"),
		Result.GetError().Contains(TEXT("Failed to load")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceClearAllMappingContextsTest,
	"UnrealMCP.Input.ClearAllMappingContexts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceClearAllMappingContextsTest::RunTest(const FString& Parameters)
{
	// Test: Clear all mapping contexts (should succeed even if no contexts exist)

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::ClearAllMappingContexts();

	// This might fail if we're not in play mode with a player controller,
	// but let's check the result appropriately
	if (Result.IsFailure())
	{
		// It's okay if it fails due to world/subsystem not being available
		TestTrue(TEXT("If it fails, should be due to world/subsystem issues"),
			Result.GetError().Contains(TEXT("world")) ||
			Result.GetError().Contains(TEXT("player controller")) ||
			Result.GetError().Contains(TEXT("subsystem")));
	}
	else
	{
		// If it succeeds, that's also fine
		TestTrue(TEXT("ClearAllMappingContexts should succeed"), Result.IsSuccess());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateLegacyInputMappingTest,
	"UnrealMCP.Input.CreateLegacyInputMapping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateLegacyInputMappingTest::RunTest(const FString& Parameters)
{
	// Test: Create a legacy input mapping

	UnrealMCP::FLegacyInputMappingParams Params;
	Params.ActionName = TEXT("TestAction");
	Params.Key = TEXT("Space");
	Params.bShift = false;
	Params.bCtrl = false;
	Params.bAlt = false;
	Params.bCmd = false;

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::CreateLegacyInputMapping(Params);

	// Verify success
	TestTrue(TEXT("CreateLegacyInputMapping should succeed"), Result.IsSuccess());

	// Note: We could verify that the mapping was actually added by checking the input settings,
	// but for a functional test, just verifying it doesn't fail is sufficient

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateLegacyInputMappingWithEmptyNameTest,
	"UnrealMCP.Input.CreateLegacyInputMappingWithEmptyName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateLegacyInputMappingWithEmptyNameTest::RunTest(const FString& Parameters)
{
	// Test: Creating legacy input mapping with empty action name should fail

	UnrealMCP::FLegacyInputMappingParams Params;
	Params.ActionName = TEXT("");  // Empty action name
	Params.Key = TEXT("Space");
	Params.bShift = false;
	Params.bCtrl = false;
	Params.bAlt = false;
	Params.bCmd = false;

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::CreateLegacyInputMapping(Params);

	// Verify failure
	TestTrue(TEXT("CreateLegacyInputMapping should fail with empty action name"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention action name cannot be empty"),
		Result.GetError().Contains(TEXT("Action name cannot be empty")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateLegacyInputMappingWithEmptyKeyTest,
	"UnrealMCP.Input.CreateLegacyInputMappingWithEmptyKey",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateLegacyInputMappingWithEmptyKeyTest::RunTest(const FString& Parameters)
{
	// Test: Creating legacy input mapping with empty key should fail

	UnrealMCP::FLegacyInputMappingParams Params;
	Params.ActionName = TEXT("TestAction");
	Params.Key = TEXT("");  // Empty key
	Params.bShift = false;
	Params.bCtrl = false;
	Params.bAlt = false;
	Params.bCmd = false;

	const UnrealMCP::FVoidResult Result = UnrealMCP::FInputService::CreateLegacyInputMapping(Params);

	// Verify failure
	TestTrue(TEXT("CreateLegacyInputMapping should fail with empty key"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention key cannot be empty"),
		Result.GetError().Contains(TEXT("Key cannot be empty")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FInputServiceCreateInputActionWithDifferentValueTypesTest,
	"UnrealMCP.Input.CreateInputActionWithDifferentValueTypes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FInputServiceCreateInputActionWithDifferentValueTypesTest::RunTest(const FString& Parameters)
{
	// Test: Create input actions with different value types

	TArray<FString> ValueTypes = { TEXT("Boolean"), TEXT("Axis1D"), TEXT("Axis2D"), TEXT("Axis3D") };
	TArray<EInputActionValueType> ExpectedTypes = {
		EInputActionValueType::Boolean,
		EInputActionValueType::Axis1D,
		EInputActionValueType::Axis2D,
		EInputActionValueType::Axis3D
	};

	for (int32 i = 0; i < ValueTypes.Num(); ++i)
	{
		UnrealMCP::FInputActionParams Params;
		Params.Name = FString::Printf(TEXT("TestAction_%d"), i);
		Params.ValueType = ValueTypes[i];
		Params.Path = TEXT("/Game/Input");

		UnrealMCP::TResult<UInputAction*> Result = UnrealMCP::FInputService::CreateInputAction(Params);

		// Verify success
		TestTrue(FString::Printf(TEXT("CreateInputAction should succeed for type %s"), *ValueTypes[i]), Result.IsSuccess());
		UInputAction* InputAction = Result.GetValue();
		TestTrue(FString::Printf(TEXT("InputAction should not be null for type %s"), *ValueTypes[i]), InputAction != nullptr);
		if (InputAction)
		{
			TestEqual(FString::Printf(TEXT("Value type should be %s"), *ValueTypes[i]),
				static_cast<uint8>(InputAction->ValueType), static_cast<uint8>(ExpectedTypes[i]));
		}

		// Cleanup
		if (InputAction)
		{
			const UPackage* Package = InputAction->GetOutermost();
			if (Package)
			{
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				FString PackagePath = Package->GetPathName();
				FString FilePath = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
				PlatformFile.DeleteFile(*FilePath);
			}
		}
	}

	return true;
}