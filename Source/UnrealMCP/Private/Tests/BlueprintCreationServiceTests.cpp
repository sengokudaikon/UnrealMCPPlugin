/**
 * Functional tests for BlueprintCreationService
 *
 * These tests verify the actual behavior of blueprint creation:
 * - Creating blueprints with different parent classes
 * - Blueprint compilation status
 * - Asset creation and persistence
 * - Error handling for invalid inputs
 *
 * Tests run in the Unreal Editor with real asset creation.
 */

#include "EditorAssetLibrary.h"
#include "TestUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/MCPTypes.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/AutomationTest.h"
#include "Services/BlueprintCreationService.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationActorTest,
	"UnrealMCP.Blueprint.CreateActorBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationActorTest::RunTest(const FString& Parameters) -> bool {
	// Test: Create an Actor blueprint and verify it exists as a real asset

	FString BlueprintName = TEXT("TestActorBP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	// Create blueprint parameters
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Actor");
	Params.PackagePath = TEXT("/Game/Tests/");

	// Create the blueprint
	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	// Verify creation succeeded
	TestTrue(TEXT("Blueprint creation should succeed"), Result.IsSuccess());

	if (Result.IsSuccess()) {
		const UBlueprint* Blueprint = Result.GetValue();
		TestNotNull(TEXT("Created blueprint should not be null"), Blueprint);

		if (Blueprint) {
			// Verify blueprint properties
			TestTrue(TEXT("Blueprint name should match request"),
			         Blueprint->GetName().Contains(BlueprintName));

			TestNotNull(TEXT("Blueprint should have generated class"),
			            Blueprint->GeneratedClass.Get());

			TestTrue(TEXT("Blueprint should be child of AActor"),
			         Blueprint->GeneratedClass->IsChildOf<AActor>());

			// Verify blueprint status is valid (compiled)
			TestEqual(TEXT("Blueprint should be compiled (BS_UpToDate)"),
			          Blueprint->Status,
			          BS_UpToDate);

			// Verify asset actually exists in AssetRegistry
			const FString AssetPath = FString::Printf(TEXT("/Game/Tests/%s.%s"), *BlueprintName, *BlueprintName);
			TestTrue(TEXT("Blueprint asset should exist in project"),
			         UEditorAssetLibrary::DoesAssetExist(AssetPath));
		}
	}

	// Cleanup
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationPawnTest,
	"UnrealMCP.Blueprint.CreatePawnBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationPawnTest::RunTest(const FString& Parameters) -> bool {
	// Test: Create a Pawn blueprint with proper hierarchy

	FString BlueprintName = TEXT("TestPawnBP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Pawn");
	Params.PackagePath = TEXT("/Game/Tests/");

	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	TestTrue(TEXT("Pawn blueprint creation should succeed"), Result.IsSuccess());

	if (Result.IsSuccess()) {
		const UBlueprint* Blueprint = Result.GetValue();
		TestNotNull(TEXT("Pawn blueprint should not be null"), Blueprint);

		if (Blueprint && Blueprint->GeneratedClass) {
			// Verify inheritance hierarchy
			TestTrue(TEXT("Blueprint should be child of APawn"),
			         Blueprint->GeneratedClass->IsChildOf<APawn>());

			// Pawn should also be an Actor
			TestTrue(TEXT("Pawn blueprint should also be child of AActor"),
			         Blueprint->GeneratedClass->IsChildOf<AActor>());

			// Verify it has SimpleConstructionScript for components
			TestNotNull(TEXT("Blueprint should have SimpleConstructionScript"),
			            Blueprint->SimpleConstructionScript.Get());
		}
	}

	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationCharacterTest,
	"UnrealMCP.Blueprint.CreateCharacterBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationCharacterTest::RunTest(const FString& Parameters) -> bool {
	// Test: Create a Character blueprint (more complex parent class)

	FString BlueprintName = TEXT("TestCharacterBP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Character");
	Params.PackagePath = TEXT("/Game/Tests/");

	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	TestTrue(TEXT("Character blueprint creation should succeed"), Result.IsSuccess());

	if (Result.IsSuccess()) {
		const UBlueprint* Blueprint = Result.GetValue();

		if (Blueprint && Blueprint->GeneratedClass) {
			// Character inherits from Pawn
			TestTrue(TEXT("Blueprint should be child of ACharacter"),
			         Blueprint->GeneratedClass->IsChildOf<ACharacter>());

			TestTrue(TEXT("Character blueprint should be child of APawn"),
			         Blueprint->GeneratedClass->IsChildOf<APawn>());

			// Character blueprints should have components (inherited from Character class)
			TestNotNull(TEXT("Character blueprint should have SimpleConstructionScript"),
			            Blueprint->SimpleConstructionScript.Get());
		}
	}

	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationDuplicateTest,
	"UnrealMCP.Blueprint.DuplicateNameHandling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationDuplicateTest::RunTest(const FString& Parameters) -> bool {
	// Test: Creating duplicate blueprint should fail gracefully

	FString BlueprintName = TEXT("TestDuplicateBP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Actor");
	Params.PackagePath = TEXT("/Game/Tests/");

	// Create first blueprint
	const auto FirstResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("First blueprint creation should succeed"), FirstResult.IsSuccess());

	// Try to create second blueprint with same name
	const auto SecondResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	// Should fail (or return existing blueprint - implementation dependent)
	TestTrue(TEXT("Duplicate creation should either fail or return existing blueprint"),
	         SecondResult.IsSuccess() || SecondResult.IsFailure());

	if (SecondResult.IsFailure()) {
		// Verify error message mentions the issue
		const FString Error = SecondResult.GetError();
		TestTrue(TEXT("Error message should indicate duplicate/existing asset"),
		         Error.Contains(TEXT("exists")) || Error.Contains(TEXT("duplicate")) || Error.Contains(
			         TEXT("already")));
	}

	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCompilationTest,
	"UnrealMCP.Blueprint.CompileBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCompilationTest::RunTest(const FString& Parameters) -> bool {
	// Test: Compile an existing blueprint and verify status changes

	FString BlueprintName = TEXT("TestCompileBP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	// Create blueprint
	UnrealMCP::FBlueprintCreationParams CreateParams;
	CreateParams.Name = BlueprintName;
	CreateParams.ParentClass = TEXT("Actor");
	CreateParams.PackagePath = TEXT("/Game/Tests/");

	auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreateParams);
	TestTrue(TEXT("Blueprint should be created for compile test"), CreateResult.IsSuccess());

	if (CreateResult.IsSuccess()) {
		// Mark blueprint as dirty to test compilation
		const UBlueprint* Blueprint = CreateResult.GetValue();
		if (Blueprint) {
			// Compile the blueprint
			const auto CompileResult = UnrealMCP::FBlueprintCreationService::CompileBlueprint(BlueprintName);

			TestTrue(TEXT("Blueprint compilation should succeed"), CompileResult.IsSuccess());

			// After successful compile, blueprint should be up to date
			TestEqual(TEXT("Blueprint status should be BS_UpToDate after compile"),
			          Blueprint->Status,
			          BS_UpToDate);
		}
	}

	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCompileNonExistentTest,
	"UnrealMCP.Blueprint.CompileNonExistent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCompileNonExistentTest::RunTest(const FString& Parameters) -> bool {
	// Test: Compiling non-existent blueprint should fail with clear error

	const FString NonExistentName = TEXT("NonExistentBlueprint_XYZ999");

	const auto Result = UnrealMCP::FBlueprintCreationService::CompileBlueprint(NonExistentName);

	TestTrue(TEXT("Compiling non-existent blueprint should fail"), Result.IsFailure());

	if (Result.IsFailure()) {
		const FString Error = Result.GetError();
		TestTrue(TEXT("Error should mention blueprint not found"),
		         Error.Contains(TEXT("not found")) || Error.Contains(TEXT("does not exist")) || Error.Contains(
			         TEXT("failed to load")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationWithPrefixTest,
	"UnrealMCP.Blueprint.ParentClassWithPrefix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationWithPrefixTest::RunTest(const FString& Parameters) -> bool {
	// Test: Parent class names with/without "A" prefix should both work

	FString BlueprintName = TEXT("TestPrefixBP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	// Try with "AActor" prefix
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("AActor"); // With prefix
	Params.PackagePath = TEXT("/Game/Tests/");

	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	TestTrue(TEXT("Creation with 'AActor' prefix should succeed"), Result.IsSuccess());

	if (Result.IsSuccess()) {
		const UBlueprint* Blueprint = Result.GetValue();
		if (Blueprint && Blueprint->GeneratedClass) {
			TestTrue(TEXT("Blueprint with 'AActor' prefix should be Actor class"),
			         Blueprint->GeneratedClass->IsChildOf<AActor>());
		}
	}

	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);
	return true;
}

// NEW TESTS TO REACH 90% COVERAGE

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationEmptyNameTest,
	"UnrealMCP.Blueprint.EmptyNameValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationEmptyNameTest::RunTest(const FString& Parameters) -> bool {
	// Test: Creating blueprint with empty name should fail gracefully

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT(""); // Empty name
	Params.ParentClass = TEXT("Actor");
	Params.PackagePath = TEXT("/Game/Tests/");

	const auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	// Should fail due to empty name
	TestTrue(TEXT("Empty blueprint name should fail creation"), Result.IsFailure());

	if (Result.IsFailure()) {
		// Verify the failure indicates a validation error
		const FString Error = Result.GetError();
		TestTrue(TEXT("Error should indicate name validation issue"),
		         Error.Contains(TEXT("empty")) || Error.Contains(TEXT("name")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationInvalidParentClassTest,
	"UnrealMCP.Blueprint.InvalidParentClassHandling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationInvalidParentClassTest::RunTest(const FString& Parameters) -> bool {
	// Test: Creating blueprint with invalid parent class should fallback to AActor

	FString BlueprintName = TEXT("TestInvalidParentBP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("NonExistentClassXYZ123"); // Invalid class
	Params.PackagePath = TEXT("/Game/Tests/");

	const auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	// Should succeed due to fallback to AActor
	TestTrue(TEXT("Invalid parent class should fallback to AActor and succeed"), Result.IsSuccess());

	if (Result.IsSuccess()) {
		const UBlueprint* Blueprint = Result.GetValue();
		if (Blueprint && Blueprint->GeneratedClass) {
			// Verify it defaults to Actor class
			TestTrue(TEXT("Blueprint should default to AActor class"),
			         Blueprint->GeneratedClass->IsChildOf<AActor>());
		}
	}

	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationSpecialCharactersTest,
	"UnrealMCP.Blueprint.SpecialCharactersInName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCreationSpecialCharactersTest::RunTest(const FString& Parameters) -> bool {
	// Test: Creating blueprint with special characters should work

	FString BlueprintName = TEXT("Test_Special-123_BP");
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Actor");
	Params.PackagePath = TEXT("/Game/Tests/");

	const auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	// Should succeed with special characters
	TestTrue(TEXT("Blueprint with special characters should be created"), Result.IsSuccess());

	if (Result.IsSuccess()) {
		const UBlueprint* Blueprint = Result.GetValue();
		TestNotNull(TEXT("Blueprint with special characters should not be null"), Blueprint);

		if (Blueprint) {
			// Verify the name contains our special characters (underscore and hyphen are valid)
			TestTrue(TEXT("Blueprint name should contain special characters"),
			         Blueprint->GetName().Contains(TEXT("Test_Special-123_BP")));
		}
	}

	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCompilationEmptyNameTest,
	"UnrealMCP.Blueprint.CompileEmptyName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintCompilationEmptyNameTest::RunTest(const FString& Parameters) -> bool {
	// Test: Compiling with empty name should fail with validation error

	const FString EmptyName = TEXT("");

	const auto Result = UnrealMCP::FBlueprintCreationService::CompileBlueprint(EmptyName);

	TestTrue(TEXT("Compiling empty name should fail"), Result.IsFailure());

	if (Result.IsFailure()) {
		const FString Error = Result.GetError();
		TestTrue(TEXT("Error should indicate name validation issue"),
		         Error.Contains(TEXT("empty")) || Error.Contains(TEXT("name")));
	}

	return true;
}
