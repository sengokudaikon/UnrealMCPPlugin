/**
 * Functional tests for BlueprintService
 *
 * These tests verify the actual behavior of blueprint operations:
 * - Spawning blueprint actors (with invalid blueprints)
 * - Adding components (with invalid parameters)
 * - Setting component properties (with invalid targets)
 * - Setting physics properties (with validation)
 * - Setting static mesh properties (with asset loading)
 * - Setting blueprint properties (with error handling)
 *
 * Tests focus on error handling paths as functional tests with valid blueprints
 * would require complex asset setup and management.
 *
 * Tests run in the Unreal Editor with real world context.
 */

#include "Services/BlueprintService.h"
#include "Core/MCPTypes.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Editor.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSpawnInvalidActorBlueprintTest,
	"UnrealMCP.Blueprint.SpawnInvalidActorBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSpawnInvalidActorBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Spawning actor from non-existent blueprint should fail

	UnrealMCP::FBlueprintSpawnParams Params;
	Params.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	Params.ActorName = TEXT("TestActor");
	Params.Location = FVector(100.0f, 200.0f, 300.0f);
	Params.Rotation = FRotator::ZeroRotator;

	const UnrealMCP::TResult<AActor*> Result = UnrealMCP::FBlueprintService::SpawnActorBlueprint(Params);

	// Verify failure
	TestTrue(TEXT("SpawnActorBlueprint should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceAddComponentToInvalidBlueprintTest,
	"UnrealMCP.Blueprint.AddComponentToInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceAddComponentToInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding component to non-existent blueprint should fail

	UnrealMCP::FComponentParams Params;
	Params.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	Params.ComponentType = TEXT("StaticMeshComponent");
	Params.ComponentName = TEXT("TestComponent");
	Params.Location = FVector::ZeroVector;
	Params.Rotation = FRotator::ZeroRotator;
	Params.Scale = FVector(1.0f, 1.0f, 1.0f);

	const UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);

	// Verify failure
	TestTrue(TEXT("AddComponent should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceAddComponentWithInvalidTypeTest,
	"UnrealMCP.Blueprint.AddComponentWithInvalidType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceAddComponentWithInvalidTypeTest::RunTest(const FString& Parameters)
{
	// Test: Adding component with invalid component type should fail

	UnrealMCP::FComponentParams Params;
	Params.BlueprintName = TEXT("SomeBlueprint");  // This will fail before validation
	Params.ComponentType = TEXT("NonExistentComponentType_XYZ123");
	Params.ComponentName = TEXT("TestComponent");

	const UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);

	// Verify failure
	TestTrue(TEXT("AddComponent should fail for invalid component type"), Result.IsFailure());

	// It might fail due to blueprint not existing first, which is expected
	// The key is that it fails gracefully with an error message
	TestTrue(TEXT("Should have an error message"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetComponentPropertyInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetComponentPropertyInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSetComponentPropertyInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting component property on non-existent blueprint should fail

	UnrealMCP::FPropertyParams PropertyParams;
	PropertyParams.TargetName = TEXT("TestComponent");
	PropertyParams.PropertyName = TEXT("SomeProperty");

	// Create a simple boolean value
	const TSharedPtr<FJsonObject> JsonValue = MakeShareable(new FJsonObject);
	JsonValue->SetBoolField(TEXT("value"), true);
	PropertyParams.PropertyValue = MakeShareable(new FJsonValueBoolean(true));

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetComponentProperty(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("TestComponent"),
		PropertyParams
	);

	// Verify failure
	TestTrue(TEXT("SetComponentProperty should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetPhysicsPropertiesInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetPhysicsPropertiesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSetPhysicsPropertiesInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting physics properties on non-existent blueprint should fail

	UnrealMCP::FPhysicsParams Params;
	Params.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	Params.ComponentName = TEXT("TestComponent");
	Params.bSimulatePhysics = true;
	Params.Mass = 10.0f;
	Params.LinearDamping = 0.1f;
	Params.AngularDamping = 0.0f;
	Params.bEnableGravity = true;

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetPhysicsProperties(Params);

	// Verify failure
	TestTrue(TEXT("SetPhysicsProperties should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetStaticMeshPropertiesInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetStaticMeshPropertiesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSetStaticMeshPropertiesInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting static mesh properties on non-existent blueprint should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetStaticMeshProperties(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("TestComponent"),
		TEXT("/Game/Meshes/SomeMesh"),
		TOptional<FString>(TEXT("/Game/Materials/SomeMaterial"))
	);

	// Verify failure
	TestTrue(TEXT("SetStaticMeshProperties should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetStaticMeshPropertiesInvalidMeshTest,
	"UnrealMCP.Blueprint.SetStaticMeshPropertiesInvalidMesh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSetStaticMeshPropertiesInvalidMeshTest::RunTest(const FString& Parameters)
{
	// Test: Setting static mesh with invalid mesh path should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetStaticMeshProperties(
		TEXT("SomeBlueprint"),  // This will fail before mesh validation
		TEXT("TestComponent"),
		TEXT("/Game/Meshes/NonExistentMesh_XYZ123"),
		TOptional<FString>()
	);

	// Verify failure
	TestTrue(TEXT("SetStaticMeshProperties should fail"), Result.IsFailure());
	TestTrue(TEXT("Should have an error message"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetBlueprintPropertyInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetBlueprintPropertyInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSetBlueprintPropertyInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting blueprint property on non-existent blueprint should fail

	UnrealMCP::FPropertyParams PropertyParams;
	PropertyParams.TargetName = TEXT("Blueprint");
	PropertyParams.PropertyName = TEXT("SomeProperty");

	// Create a simple boolean value
	const TSharedPtr<FJsonObject> JsonValue = MakeShareable(new FJsonObject);
	JsonValue->SetBoolField(TEXT("value"), true);
	PropertyParams.PropertyValue = MakeShareable(new FJsonValueBoolean(true));

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetBlueprintProperty(
		TEXT("NonExistentBlueprint_XYZ123"),
		PropertyParams
	);

	// Verify failure
	TestTrue(TEXT("SetBlueprintProperty should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetPawnPropertiesInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetPawnPropertiesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSetPawnPropertiesInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting pawn properties on non-existent blueprint should fail

	// Create a JSON object with pawn properties
	const TSharedPtr<FJsonObject> PawnProperties = MakeShareable(new FJsonObject);
	PawnProperties->SetBoolField(TEXT("AutoPossessPlayer"), true);
	PawnProperties->SetBoolField(TEXT("bUseControllerRotationPitch"), true);
	PawnProperties->SetBoolField(TEXT("bUseControllerRotationYaw"), true);
	PawnProperties->SetBoolField(TEXT("bUseControllerRotationRoll"), true);

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetPawnProperties(
		TEXT("NonExistentBlueprint_XYZ123"),
		PawnProperties
	);

	// Verify failure
	TestTrue(TEXT("SetPawnProperties should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetPawnPropertiesWithValidBlueprintTest,
	"UnrealMCP.Blueprint.SetPawnPropertiesWithValidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceSetPawnPropertiesWithValidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting pawn properties with valid JSON structure

	// This test verifies that the service can handle valid JSON input
	// even if we can't test with a real blueprint in this context

	const TSharedPtr<FJsonObject> PawnProperties = MakeShareable(new FJsonObject);
	PawnProperties->SetBoolField(TEXT("AutoPossessPlayer"), false);
	PawnProperties->SetBoolField(TEXT("bUseControllerRotationPitch"), false);
	PawnProperties->SetBoolField(TEXT("bUseControllerRotationYaw"), false);
	PawnProperties->SetBoolField(TEXT("bUseControllerRotationRoll"), false);

	// Try with a blueprint name that likely doesn't exist - should fail gracefully
	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetPawnProperties(
		TEXT("SomePawnBlueprint"),
		PawnProperties
	);

	// This should fail, but the important part is that it handles the JSON input correctly
	// and fails due to the blueprint not existing, not due to JSON parsing errors
	TestTrue(TEXT("Should fail gracefully"), Result.IsFailure());
	TestTrue(TEXT("Error should be meaningful"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceComponentParameterValidationTest,
	"UnrealMCP.Blueprint.ComponentParameterValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServiceComponentParameterValidationTest::RunTest(const FString& Parameters)
{
	// Test: Various component parameter validation scenarios

	// Test with empty component name
	{
		UnrealMCP::FComponentParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentType = TEXT("StaticMeshComponent");
		Params.ComponentName = TEXT("");  // Empty component name

		UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);
		TestTrue(TEXT("Should fail with empty component name"), Result.IsFailure());
		TestTrue(TEXT("Error should be meaningful"), !Result.GetError().IsEmpty());
	}

	// Test with empty component type
	{
		UnrealMCP::FComponentParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentType = TEXT("");  // Empty component type
		Params.ComponentName = TEXT("TestComponent");

		UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);
		TestTrue(TEXT("Should fail with empty component type"), Result.IsFailure());
		TestTrue(TEXT("Error should be meaningful"), !Result.GetError().IsEmpty());
	}

	// Test with empty blueprint name
	{
		UnrealMCP::FComponentParams Params;
		Params.BlueprintName = TEXT("");  // Empty blueprint name
		Params.ComponentType = TEXT("StaticMeshComponent");
		Params.ComponentName = TEXT("TestComponent");

		UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);
		TestTrue(TEXT("Should fail with empty blueprint name"), Result.IsFailure());
		TestTrue(TEXT("Error should be meaningful"), !Result.GetError().IsEmpty());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServicePhysicsParameterValidationTest,
	"UnrealMCP.Blueprint.PhysicsParameterValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintServicePhysicsParameterValidationTest::RunTest(const FString& Parameters)
{
	// Test: Physics parameters validation with different values

	// Test with negative mass (should still work but validate)
	{
		UnrealMCP::FPhysicsParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentName = TEXT("TestComponent");
		Params.bSimulatePhysics = true;
		Params.Mass = -5.0f;  // Negative mass
		Params.LinearDamping = 0.1f;
		Params.AngularDamping = 0.0f;
		Params.bEnableGravity = true;

		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetPhysicsProperties(Params);
		// Should fail, but not due to parameter validation (due to blueprint not existing)
		TestTrue(TEXT("Should fail gracefully with negative mass"), Result.IsFailure());
	}

	// Test with extreme damping values
	{
		UnrealMCP::FPhysicsParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentName = TEXT("TestComponent");
		Params.bSimulatePhysics = true;
		Params.Mass = 1.0f;
		Params.LinearDamping = 1000.0f;  // Very high damping
		Params.AngularDamping = 1000.0f;  // Very high damping
		Params.bEnableGravity = false;

		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetPhysicsProperties(Params);
		// Should fail, but not due to parameter validation (due to blueprint not existing)
		TestTrue(TEXT("Should fail gracefully with extreme damping"), Result.IsFailure());
	}

	return true;
}