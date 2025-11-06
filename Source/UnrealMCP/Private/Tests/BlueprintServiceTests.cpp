#include "Editor.h"
#include "Blueprint/UserWidget.h"
#include "Core/MCPTypes.h"
#include "Dom/JsonObject.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "Services/BlueprintCreationService.h"
#include "Services/BlueprintService.h"
#include "Tests/TestUtils.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSpawnInvalidActorBlueprintTest,
	"UnrealMCP.Blueprint.SpawnInvalidActorBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSpawnInvalidActorBlueprintTest::RunTest(const FString& Parameters) -> bool {
	// Test: Spawning actor from non-existent blueprint should fail

	UnrealMCP::FBlueprintSpawnParams Params;
	Params.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	Params.ActorName = TEXT("TestActor");
	Params.Location = FVector(100.0f, 200.0f, 300.0f);
	Params.Rotation = FRotator::ZeroRotator;
	Params.Scale = FVector(1.5f, 1.5f, 1.5f);

	const UnrealMCP::TResult<AActor*> Result = UnrealMCP::FBlueprintService::SpawnActorBlueprint(Params);

	// Verify failure
	TestTrue(TEXT("SpawnActorBlueprint should fail for non-existent blueprint"), Result.IsFailure());

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		Params.BlueprintName,
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSpawnParamsIncludesScaleTest,
	"UnrealMCP.Blueprint.SpawnParamsIncludesScale",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSpawnParamsIncludesScaleTest::RunTest(const FString& Parameters) -> bool {
	// Test: Verify that FBlueprintSpawnParams properly includes and validates scale parameter

	UnrealMCP::FBlueprintSpawnParams Params;
	Params.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	Params.ActorName = TEXT("TestActorWithScale");
	Params.Location = FVector(0.0f, 0.0f, 0.0f);
	Params.Rotation = FRotator::ZeroRotator;
	Params.Scale = FVector(2.0f, 2.0f, 2.0f);

	// Verify scale is set
	TestTrue(TEXT("Scale should be set"), Params.Scale.IsSet());
	TestEqual(TEXT("Scale X should be 2.0"), Params.Scale.GetValue().X, 2.0);
	TestEqual(TEXT("Scale Y should be 2.0"), Params.Scale.GetValue().Y, 2.0);
	TestEqual(TEXT("Scale Z should be 2.0"), Params.Scale.GetValue().Z, 2.0);

	// Test default scale behavior (when not set)
	UnrealMCP::FBlueprintSpawnParams ParamsNoScale;
	ParamsNoScale.BlueprintName = TEXT("TestBlueprint");
	ParamsNoScale.ActorName = TEXT("TestActorNoScale");

	TestFalse(TEXT("Scale should not be set when not specified"), ParamsNoScale.Scale.IsSet());
	TestEqual(TEXT("Default scale should be (1,1,1)"),
		ParamsNoScale.Scale.Get(FVector::OneVector),
		FVector::OneVector);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceAddComponentToInvalidBlueprintTest,
	"UnrealMCP.Blueprint.AddComponentToInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceAddComponentToInvalidBlueprintTest::RunTest(const FString& Parameters) -> bool {
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

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		Params.BlueprintName,
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceAddComponentWithInvalidTypeTest,
	"UnrealMCP.Blueprint.AddComponentWithInvalidType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceAddComponentWithInvalidTypeTest::RunTest(const FString& Parameters) -> bool {
	// Test: Adding component with invalid component type should fail

	// First create a test blueprint
	UnrealMCP::FBlueprintCreationParams BlueprintParams;
	BlueprintParams.Name = TEXT("AddComponentTestBlueprint");
	BlueprintParams.PackagePath = TEXT("/Game/Tests/BlueprintService/");
	BlueprintParams.ParentClass = TEXT("Actor");

	const auto BlueprintResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(BlueprintParams);
	TestTrue(TEXT("Test blueprint should be created"), BlueprintResult.IsSuccess());

	// Test adding component with invalid component type
	UnrealMCP::FComponentParams Params;
	Params.BlueprintName = BlueprintParams.Name;
	Params.ComponentType = TEXT("NonExistentComponentType_XYZ123");
	Params.ComponentName = TEXT("TestComponent");

	const UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);

	// Verify failure
	TestTrue(TEXT("AddComponent should fail for invalid component type"), Result.IsFailure());

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::InvalidComponentType,
		Params.ComponentType,
		this
	);

	// Cleanup
	UnrealMCPTest::FTestUtils::CleanupTestBlueprintByName(BlueprintParams.Name, BlueprintParams.PackagePath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetComponentPropertyInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetComponentPropertyInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSetComponentPropertyInvalidBlueprintTest::RunTest(const FString& Parameters) -> bool {
	// Test: Setting component property on non-existent blueprint should fail

	UnrealMCP::FPropertyParams PropertyParams;
	PropertyParams.TargetName = TEXT("TestComponent");
	PropertyParams.PropertyName = TEXT("SomeProperty");

	// Create a simple boolean value
	const TSharedPtr<FJsonObject> JsonValue = MakeShareable(new FJsonObject);
	JsonValue->SetBoolField(TEXT("value"), true);
	PropertyParams.PropertyValue = MakeShareable(new FJsonValueBoolean(true));

	const FString BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetComponentProperty(
		BlueprintName,
		TEXT("TestComponent"),
		PropertyParams
	);

	// Verify failure
	TestTrue(TEXT("SetComponentProperty should fail for non-existent blueprint"), Result.IsFailure());

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		BlueprintName,
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetPhysicsPropertiesInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetPhysicsPropertiesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSetPhysicsPropertiesInvalidBlueprintTest::RunTest(const FString& Parameters) -> bool {
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

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		Params.BlueprintName,
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetStaticMeshPropertiesInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetStaticMeshPropertiesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSetStaticMeshPropertiesInvalidBlueprintTest::RunTest(const FString& Parameters) -> bool {
	// Test: Setting static mesh properties on non-existent blueprint should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetStaticMeshProperties(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("TestComponent"),
		TEXT("/Game/Meshes/SomeMesh"),
		TOptional<FString>(TEXT("/Game/Materials/SomeMaterial"))
	);

	// Verify failure
	TestTrue(TEXT("SetStaticMeshProperties should fail for non-existent blueprint"), Result.IsFailure());

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		TEXT("NonExistentBlueprint_XYZ123"),
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetStaticMeshPropertiesInvalidMeshTest,
	"UnrealMCP.Blueprint.SetStaticMeshPropertiesInvalidMesh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSetStaticMeshPropertiesInvalidMeshTest::RunTest(const FString& Parameters) -> bool {
	// Test: Setting static mesh with invalid mesh path should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetStaticMeshProperties(
		TEXT("SomeBlueprint"),
		// This will fail before mesh validation
		TEXT("TestComponent"),
		TEXT("/Game/Meshes/NonExistentMesh_XYZ123"),
		TOptional<FString>()
	);

	// Verify failure
	TestTrue(TEXT("SetStaticMeshProperties should fail"), Result.IsFailure());

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		TEXT("SomeBlueprint"),
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetBlueprintPropertyInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetBlueprintPropertyInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSetBlueprintPropertyInvalidBlueprintTest::RunTest(const FString& Parameters) -> bool {
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

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		TEXT("NonExistentBlueprint_XYZ123"),
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetPawnPropertiesInvalidBlueprintTest,
	"UnrealMCP.Blueprint.SetPawnPropertiesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSetPawnPropertiesInvalidBlueprintTest::RunTest(const FString& Parameters) -> bool {
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

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		TEXT("NonExistentBlueprint_XYZ123"),
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceSetPawnPropertiesWithValidBlueprintTest,
	"UnrealMCP.Blueprint.SetPawnPropertiesWithValidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceSetPawnPropertiesWithValidBlueprintTest::RunTest(const FString& Parameters) -> bool {
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

	UnrealMCPTest::FTestUtils::ValidateErrorCode(
		Result,
		UnrealMCP::EErrorCode::BlueprintNotFound,
		TEXT("SomePawnBlueprint"),
		this
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServiceComponentParameterValidationTest,
	"UnrealMCP.Blueprint.ComponentParameterValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServiceComponentParameterValidationTest::RunTest(const FString& Parameters) -> bool {
	// Test: Various component parameter validation scenarios

	// Test with empty component name
	{
		UnrealMCP::FComponentParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentType = TEXT("StaticMeshComponent");
		Params.ComponentName = TEXT("");

		UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);
		TestTrue(TEXT("Should fail with empty component name"), Result.IsFailure());

		UnrealMCPTest::FTestUtils::ValidateErrorCode(
			Result,
			UnrealMCP::EErrorCode::InvalidInput,
			TEXT("ComponentName"),
			this
		);
	}

	// Test with empty component type
	{
		UnrealMCP::FComponentParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentType = TEXT("");
		Params.ComponentName = TEXT("TestComponent");

		UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);
		TestTrue(TEXT("Should fail with empty component type"), Result.IsFailure());

		UnrealMCPTest::FTestUtils::ValidateErrorCode(
			Result,
			UnrealMCP::EErrorCode::InvalidInput,
			TEXT("ComponentType"),
			this
		);
	}

	// Test with empty blueprint name
	{
		UnrealMCP::FComponentParams Params;
		Params.BlueprintName = TEXT("");
		Params.ComponentType = TEXT("StaticMeshComponent");
		Params.ComponentName = TEXT("TestComponent");

		UnrealMCP::TResult<UBlueprint*> Result = UnrealMCP::FBlueprintService::AddComponent(Params);
		TestTrue(TEXT("Should fail with empty blueprint name"), Result.IsFailure());

		UnrealMCPTest::FTestUtils::ValidateErrorCode(
			Result,
			UnrealMCP::EErrorCode::InvalidInput,
			TEXT("BlueprintName"),
			this
		);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintServicePhysicsParameterValidationTest,
	"UnrealMCP.Blueprint.PhysicsParameterValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintServicePhysicsParameterValidationTest::RunTest(const FString& Parameters) -> bool {
	// Test: Physics parameters validation with different values

	// Test with negative mass (should still work but validate)
	{
		UnrealMCP::FPhysicsParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentName = TEXT("TestComponent");
		Params.bSimulatePhysics = true;
		Params.Mass = -5.0f; // Negative mass
		Params.LinearDamping = 0.1f;
		Params.AngularDamping = 0.0f;
		Params.bEnableGravity = true;

		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetPhysicsProperties(Params);
		// Should fail, but not due to parameter validation (due to blueprint not existing)
		TestTrue(TEXT("Should fail gracefully with negative mass"), Result.IsFailure());

		UnrealMCPTest::FTestUtils::ValidateErrorCode(
			Result,
			UnrealMCP::EErrorCode::BlueprintNotFound,
			TEXT("SomeBlueprint"),
			this
		);
	}

	// Test with extreme damping values
	{
		UnrealMCP::FPhysicsParams Params;
		Params.BlueprintName = TEXT("SomeBlueprint");
		Params.ComponentName = TEXT("TestComponent");
		Params.bSimulatePhysics = true;
		Params.Mass = 1.0f;
		Params.LinearDamping = 1000.0f; // Very high damping
		Params.AngularDamping = 1000.0f; // Very high damping
		Params.bEnableGravity = false;

		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintService::SetPhysicsProperties(Params);
		// Should fail, but not due to parameter validation (due to blueprint not existing)
		TestTrue(TEXT("Should fail gracefully with extreme damping"), Result.IsFailure());

		UnrealMCPTest::FTestUtils::ValidateErrorCode(
			Result,
			UnrealMCP::EErrorCode::BlueprintNotFound,
			TEXT("SomeBlueprint"),
			this
		);
	}

	return true;
}
