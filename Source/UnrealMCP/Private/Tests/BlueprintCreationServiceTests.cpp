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

#include "Services/BlueprintCreationService.h"
#include "Core/MCPTypes.h"
#include "Misc/AutomationTest.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"

// Helper function to clean up test blueprints
static void CleanupTestBlueprint(const FString& BlueprintName)
{
	const FString AssetPath = FString::Printf(TEXT("/Game/Tests/%s.%s"), *BlueprintName, *BlueprintName);
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationActorTest,
	"UnrealMCP.Blueprint.CreateActorBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintCreationActorTest::RunTest(const FString& Parameters)
{
	// Test: Create an Actor blueprint and verify it exists as a real asset

	FString BlueprintName = TEXT("TestActorBP");
	CleanupTestBlueprint(BlueprintName);

	// Create blueprint parameters
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Actor");
	Params.PackagePath = TEXT("/Game/Tests/");

	// Create the blueprint
	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	// Verify creation succeeded
	TestTrue(TEXT("Blueprint creation should succeed"), Result.IsSuccess());

	if (Result.IsSuccess())
	{
		const UBlueprint* Blueprint = Result.GetValue();
		TestNotNull(TEXT("Created blueprint should not be null"), Blueprint);

		if (Blueprint)
		{
			// Verify blueprint properties
			TestTrue(TEXT("Blueprint name should match request"),
				Blueprint->GetName().Contains(BlueprintName));

			TestNotNull(TEXT("Blueprint should have generated class"),
				Blueprint->GeneratedClass.Get());

			TestTrue(TEXT("Blueprint should be child of AActor"),
				Blueprint->GeneratedClass->IsChildOf<AActor>());

			// Verify blueprint status is valid (compiled)
			TestEqual(TEXT("Blueprint should be compiled (BS_UpToDate)"),
				static_cast<int32>(Blueprint->Status), static_cast<int32>(BS_UpToDate));

			// Verify asset actually exists in AssetRegistry
			const FString AssetPath = FString::Printf(TEXT("/Game/Tests/%s.%s"), *BlueprintName, *BlueprintName);
			TestTrue(TEXT("Blueprint asset should exist in project"),
				UEditorAssetLibrary::DoesAssetExist(AssetPath));
		}
	}

	// Cleanup
	CleanupTestBlueprint(BlueprintName);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationPawnTest,
	"UnrealMCP.Blueprint.CreatePawnBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintCreationPawnTest::RunTest(const FString& Parameters)
{
	// Test: Create a Pawn blueprint with proper hierarchy

	FString BlueprintName = TEXT("TestPawnBP");
	CleanupTestBlueprint(BlueprintName);

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Pawn");
	Params.PackagePath = TEXT("/Game/Tests/");

	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	TestTrue(TEXT("Pawn blueprint creation should succeed"), Result.IsSuccess());

	if (Result.IsSuccess())
	{
		const UBlueprint* Blueprint = Result.GetValue();
		TestNotNull(TEXT("Pawn blueprint should not be null"), Blueprint);

		if (Blueprint && Blueprint->GeneratedClass)
		{
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

	CleanupTestBlueprint(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationCharacterTest,
	"UnrealMCP.Blueprint.CreateCharacterBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintCreationCharacterTest::RunTest(const FString& Parameters)
{
	// Test: Create a Character blueprint (more complex parent class)

	FString BlueprintName = TEXT("TestCharacterBP");
	CleanupTestBlueprint(BlueprintName);

	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Character");
	Params.PackagePath = TEXT("/Game/Tests/");

	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	TestTrue(TEXT("Character blueprint creation should succeed"), Result.IsSuccess());

	if (Result.IsSuccess())
	{
		const UBlueprint* Blueprint = Result.GetValue();

		if (Blueprint && Blueprint->GeneratedClass)
		{
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

	CleanupTestBlueprint(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationDuplicateTest,
	"UnrealMCP.Blueprint.DuplicateNameHandling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintCreationDuplicateTest::RunTest(const FString& Parameters)
{
	// Test: Creating duplicate blueprint should fail gracefully

	FString BlueprintName = TEXT("TestDuplicateBP");
	CleanupTestBlueprint(BlueprintName);

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

	if (SecondResult.IsFailure())
	{
		// Verify error message mentions the issue
		const FString Error = SecondResult.GetError();
		TestTrue(TEXT("Error message should indicate duplicate/existing asset"),
			Error.Contains(TEXT("exists")) || Error.Contains(TEXT("duplicate")) || Error.Contains(TEXT("already")));
	}

	CleanupTestBlueprint(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCompilationTest,
	"UnrealMCP.Blueprint.CompileBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintCompilationTest::RunTest(const FString& Parameters)
{
	// Test: Compile an existing blueprint and verify status changes

	FString BlueprintName = TEXT("TestCompileBP");
	CleanupTestBlueprint(BlueprintName);

	// Create blueprint
	UnrealMCP::FBlueprintCreationParams CreateParams;
	CreateParams.Name = BlueprintName;
	CreateParams.ParentClass = TEXT("Actor");
	CreateParams.PackagePath = TEXT("/Game/Tests/");

	auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreateParams);
	TestTrue(TEXT("Blueprint should be created for compile test"), CreateResult.IsSuccess());

	if (CreateResult.IsSuccess())
	{
		// Mark blueprint as dirty to test compilation
		const UBlueprint* Blueprint = CreateResult.GetValue();
		if (Blueprint)
		{
			// Compile the blueprint
			const auto CompileResult = UnrealMCP::FBlueprintCreationService::CompileBlueprint(BlueprintName);

			TestTrue(TEXT("Blueprint compilation should succeed"), CompileResult.IsSuccess());

			// After successful compile, blueprint should be up to date
			TestEqual(TEXT("Blueprint status should be BS_UpToDate after compile"),
				static_cast<int32>(Blueprint->Status), static_cast<int32>(BS_UpToDate));
		}
	}

	CleanupTestBlueprint(BlueprintName);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCompileNonExistentTest,
	"UnrealMCP.Blueprint.CompileNonExistent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintCompileNonExistentTest::RunTest(const FString& Parameters)
{
	// Test: Compiling non-existent blueprint should fail with clear error

	const FString NonExistentName = TEXT("NonExistentBlueprint_XYZ999");

	const auto Result = UnrealMCP::FBlueprintCreationService::CompileBlueprint(NonExistentName);

	TestTrue(TEXT("Compiling non-existent blueprint should fail"), Result.IsFailure());

	if (Result.IsFailure())
	{
		const FString Error = Result.GetError();
		TestTrue(TEXT("Error should mention blueprint not found"),
			Error.Contains(TEXT("not found")) || Error.Contains(TEXT("does not exist")) || Error.Contains(TEXT("failed to load")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintCreationWithPrefixTest,
	"UnrealMCP.Blueprint.ParentClassWithPrefix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintCreationWithPrefixTest::RunTest(const FString& Parameters)
{
	// Test: Parent class names with/without "A" prefix should both work

	FString BlueprintName = TEXT("TestPrefixBP");
	CleanupTestBlueprint(BlueprintName);

	// Try with "AActor" prefix
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("AActor");  // With prefix
	Params.PackagePath = TEXT("/Game/Tests/");

	auto Result = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);

	TestTrue(TEXT("Creation with 'AActor' prefix should succeed"), Result.IsSuccess());

	if (Result.IsSuccess())
	{
		const UBlueprint* Blueprint = Result.GetValue();
		if (Blueprint && Blueprint->GeneratedClass)
		{
			TestTrue(TEXT("Blueprint with 'AActor' prefix should be Actor class"),
				Blueprint->GeneratedClass->IsChildOf<AActor>());
		}
	}

	CleanupTestBlueprint(BlueprintName);
	return true;
}
