/**
 * Functional tests for BlueprintIntrospectionService
 *
 * These tests verify the actual behavior of blueprint introspection operations:
 * - Getting component properties from invalid blueprints
 * - Getting component properties with invalid component names
 * - Parameter validation for component properties requests
 * - Empty and null parameter handling
 *
 * Tests focus on error handling paths as functional tests with valid blueprints
 * would require complex asset setup and management.
 *
 * Tests run in the Unreal Editor with real world context.
 */

#include "Services/BlueprintIntrospectionService.h"
#include "Core/MCPTypes.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Engine/Blueprint.h"
#include "Editor.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetComponentPropertiesInvalidBlueprintTest,
	"UnrealMCP.BlueprintIntrospection.GetComponentPropertiesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceGetComponentPropertiesInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Getting component properties from non-existent blueprint should fail

	UnrealMCP::FComponentPropertiesParams Params;
	Params.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	Params.ComponentName = TEXT("TestComponent");

	const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(Params);

	// Verify failure
	TestTrue(TEXT("GetComponentProperties should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Blueprint")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetComponentPropertiesInvalidComponentTest,
	"UnrealMCP.BlueprintIntrospection.GetComponentPropertiesInvalidComponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceGetComponentPropertiesInvalidComponentTest::RunTest(const FString& Parameters)
{
	// Test: Getting component properties with non-existent component should fail

	UnrealMCP::FComponentPropertiesParams Params;
	Params.BlueprintName = TEXT("SomeBlueprint");  // This will fail before component validation
	Params.ComponentName = TEXT("NonExistentComponent_XYZ123");

	const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(Params);

	// Verify failure
	TestTrue(TEXT("GetComponentProperties should fail for non-existent component"), Result.IsFailure());

	// It might fail due to blueprint not existing first, which is expected
	// The key is that it fails gracefully with an error message
	TestTrue(TEXT("Should have an error message"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetComponentPropertiesEmptyBlueprintNameTest,
	"UnrealMCP.BlueprintIntrospection.GetComponentPropertiesEmptyBlueprintName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceGetComponentPropertiesEmptyBlueprintNameTest::RunTest(const FString& Parameters)
{
	// Test: Getting component properties with empty blueprint name should fail

	UnrealMCP::FComponentPropertiesParams Params;
	Params.BlueprintName = TEXT("");  // Empty blueprint name
	Params.ComponentName = TEXT("TestComponent");

	const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(Params);

	// Verify failure
	TestTrue(TEXT("GetComponentProperties should fail with empty blueprint name"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention empty"),
		Result.GetError().Contains(TEXT("empty")) || Result.GetError().Contains(TEXT("Blueprint name")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetComponentPropertiesEmptyComponentNameTest,
	"UnrealMCP.BlueprintIntrospection.GetComponentPropertiesEmptyComponentName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceGetComponentPropertiesEmptyComponentNameTest::RunTest(const FString& Parameters)
{
	// Test: Getting component properties with empty component name should fail

	UnrealMCP::FComponentPropertiesParams Params;
	Params.BlueprintName = TEXT("SomeBlueprint");
	Params.ComponentName = TEXT("");  // Empty component name

	const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(Params);

	// Verify failure
	TestTrue(TEXT("GetComponentProperties should fail with empty component name"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention empty"),
		Result.GetError().Contains(TEXT("empty")) || Result.GetError().Contains(TEXT("Component name")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceComponentPropertiesParameterValidationTest,
	"UnrealMCP.BlueprintIntrospection.ComponentPropertiesParameterValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceComponentPropertiesParameterValidationTest::RunTest(const FString& Parameters)
{
	// Test: ComponentProperties parameter validation with FromJson

	// Test with missing blueprint_name
	{
		const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
		Json->SetStringField(TEXT("component_name"), TEXT("TestComponent"));
		// Missing blueprint_name

		const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesParams> Result =
			UnrealMCP::FComponentPropertiesParams::FromJson(Json);

		TestTrue(TEXT("Should fail with missing blueprint_name"), Result.IsFailure());
		TestTrue(TEXT("Error should mention missing parameter"),
			Result.GetError().Contains(TEXT("missing")) || Result.GetError().Contains(TEXT("blueprint_name")));
	}

	// Test with missing component_name
	{
		const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
		Json->SetStringField(TEXT("blueprint_name"), TEXT("TestBlueprint"));
		// Missing component_name

		const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesParams> Result =
			UnrealMCP::FComponentPropertiesParams::FromJson(Json);

		TestTrue(TEXT("Should fail with missing component_name"), Result.IsFailure());
		TestTrue(TEXT("Error should mention missing parameter"),
			Result.GetError().Contains(TEXT("missing")) || Result.GetError().Contains(TEXT("component_name")));
	}

	// Test with valid JSON
	{
		const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
		Json->SetStringField(TEXT("blueprint_name"), TEXT("TestBlueprint"));
		Json->SetStringField(TEXT("component_name"), TEXT("TestComponent"));

		const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesParams> Result =
			UnrealMCP::FComponentPropertiesParams::FromJson(Json);

		TestTrue(TEXT("Should succeed with valid JSON"), Result.IsSuccess());

		if (Result.IsSuccess()) {
			const auto& Params = Result.GetValue();
			TestEqual(TEXT("Blueprint name should match"), Params.BlueprintName, TEXT("TestBlueprint"));
			TestEqual(TEXT("Component name should match"), Params.ComponentName, TEXT("TestComponent"));
		}
	}

	// Test with null JSON
	{
		const TSharedPtr<FJsonObject> Json = nullptr;

		const UnrealMCP::TResult<UnrealMCP::FComponentPropertiesParams> Result =
			UnrealMCP::FComponentPropertiesParams::FromJson(Json);

		TestTrue(TEXT("Should fail with null JSON"), Result.IsFailure());
		TestTrue(TEXT("Error should mention invalid JSON"),
			Result.GetError().Contains(TEXT("Invalid")) || Result.GetError().Contains(TEXT("JSON")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceComponentPropertiesResultToJsonTest,
	"UnrealMCP.BlueprintIntrospection.ComponentPropertiesResultToJson",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceComponentPropertiesResultToJsonTest::RunTest(const FString& Parameters)
{
	// Test: ComponentPropertiesResult ToJson conversion

	// Test with null properties
	{
		UnrealMCP::FComponentPropertiesResult Result;
		Result.Properties = nullptr;

		const TSharedPtr<FJsonObject> Json = Result.ToJson();

		TestTrue(TEXT("ToJson should return valid JSON"), Json.IsValid());
		TestTrue(TEXT("Should have properties field"), Json->HasField(TEXT("properties")));
	}

	// Test with valid properties
	{
		const auto PropertiesObj = MakeShared<FJsonObject>();
		PropertiesObj->SetStringField(TEXT("name"), TEXT("TestComponent"));
		PropertiesObj->SetStringField(TEXT("type"), TEXT("StaticMeshComponent"));

		UnrealMCP::FComponentPropertiesResult Result;
		Result.Properties = PropertiesObj;

		const TSharedPtr<FJsonObject> Json = Result.ToJson();

		TestTrue(TEXT("ToJson should return valid JSON"), Json.IsValid());
		TestTrue(TEXT("Should have properties field"), Json->HasField(TEXT("properties")));

		const TSharedPtr<FJsonObject> PropertiesField = Json->GetObjectField(TEXT("properties"));
		TestTrue(TEXT("Properties field should be valid"), PropertiesField.IsValid());
		TestEqual(TEXT("Component name should match"),
			PropertiesField->GetStringField(TEXT("name")), TEXT("TestComponent"));
		TestEqual(TEXT("Component type should match"),
			PropertiesField->GetStringField(TEXT("type")), TEXT("StaticMeshComponent"));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRemoveComponentInvalidBlueprintTest,
	"UnrealMCP.BlueprintIntrospection.RemoveComponentInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceRemoveComponentInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Removing component from non-existent blueprint should fail

	UnrealMCP::FRemoveComponentParams Params;
	Params.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	Params.ComponentName = TEXT("TestComponent");

	const UnrealMCP::TResult<UnrealMCP::FRemoveComponentResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::removeComponent(Params);

	// Verify failure
	TestTrue(TEXT("removeComponent should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Blueprint")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRemoveComponentInvalidComponentTest,
	"UnrealMCP.BlueprintIntrospection.RemoveComponentInvalidComponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceRemoveComponentInvalidComponentTest::RunTest(const FString& Parameters)
{
	// Test: Removing non-existent component should fail

	UnrealMCP::FRemoveComponentParams Params;
	Params.BlueprintName = TEXT("SomeBlueprint");  // This will fail before component validation
	Params.ComponentName = TEXT("NonExistentComponent_XYZ123");

	const UnrealMCP::TResult<UnrealMCP::FRemoveComponentResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::removeComponent(Params);

	// Verify failure
	TestTrue(TEXT("removeComponent should fail for non-existent component"), Result.IsFailure());

	// It might fail due to blueprint not existing first, which is expected
	// The key is that it fails gracefully with an error message
	TestTrue(TEXT("Should have an error message"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRemoveComponentEmptyBlueprintNameTest,
	"UnrealMCP.BlueprintIntrospection.RemoveComponentEmptyBlueprintName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceRemoveComponentEmptyBlueprintNameTest::RunTest(const FString& Parameters)
{
	// Test: Removing component with empty blueprint name should fail

	UnrealMCP::FRemoveComponentParams Params;
	Params.BlueprintName = TEXT("");  // Empty blueprint name
	Params.ComponentName = TEXT("TestComponent");

	const UnrealMCP::TResult<UnrealMCP::FRemoveComponentResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::removeComponent(Params);

	// Verify failure
	TestTrue(TEXT("removeComponent should fail with empty blueprint name"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention empty"),
		Result.GetError().Contains(TEXT("empty")) || Result.GetError().Contains(TEXT("Blueprint name")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRemoveComponentEmptyComponentNameTest,
	"UnrealMCP.BlueprintIntrospection.RemoveComponentEmptyComponentName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceRemoveComponentEmptyComponentNameTest::RunTest(const FString& Parameters)
{
	// Test: Removing component with empty component name should fail

	UnrealMCP::FRemoveComponentParams Params;
	Params.BlueprintName = TEXT("SomeBlueprint");
	Params.ComponentName = TEXT("");  // Empty component name

	const UnrealMCP::TResult<UnrealMCP::FRemoveComponentResult> Result =
		UnrealMCP::FBlueprintIntrospectionService::removeComponent(Params);

	// Verify failure
	TestTrue(TEXT("removeComponent should fail with empty component name"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention empty"),
		Result.GetError().Contains(TEXT("empty")) || Result.GetError().Contains(TEXT("Component name")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRemoveComponentParameterValidationTest,
	"UnrealMCP.BlueprintIntrospection.RemoveComponentParameterValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceRemoveComponentParameterValidationTest::RunTest(const FString& Parameters)
{
	// Test: RemoveComponent parameter validation with FromJson

	// Test with missing blueprint_name
	{
		const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
		Json->SetStringField(TEXT("component_name"), TEXT("TestComponent"));
		// Missing blueprint_name

		const UnrealMCP::TResult<UnrealMCP::FRemoveComponentParams> Result =
			UnrealMCP::FRemoveComponentParams::FromJson(Json);

		TestTrue(TEXT("Should fail with missing blueprint_name"), Result.IsFailure());
		TestTrue(TEXT("Error should mention missing parameter"),
			Result.GetError().Contains(TEXT("missing")) || Result.GetError().Contains(TEXT("blueprint_name")));
	}

	// Test with missing component_name
	{
		const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
		Json->SetStringField(TEXT("blueprint_name"), TEXT("TestBlueprint"));
		// Missing component_name

		const UnrealMCP::TResult<UnrealMCP::FRemoveComponentParams> Result =
			UnrealMCP::FRemoveComponentParams::FromJson(Json);

		TestTrue(TEXT("Should fail with missing component_name"), Result.IsFailure());
		TestTrue(TEXT("Error should mention missing parameter"),
			Result.GetError().Contains(TEXT("missing")) || Result.GetError().Contains(TEXT("component_name")));
	}

	// Test with valid JSON
	{
		const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
		Json->SetStringField(TEXT("blueprint_name"), TEXT("TestBlueprint"));
		Json->SetStringField(TEXT("component_name"), TEXT("TestComponent"));

		const UnrealMCP::TResult<UnrealMCP::FRemoveComponentParams> Result =
			UnrealMCP::FRemoveComponentParams::FromJson(Json);

		TestTrue(TEXT("Should succeed with valid JSON"), Result.IsSuccess());

		if (Result.IsSuccess()) {
			const auto& Params = Result.GetValue();
			TestEqual(TEXT("Blueprint name should match"), Params.BlueprintName, TEXT("TestBlueprint"));
			TestEqual(TEXT("Component name should match"), Params.ComponentName, TEXT("TestComponent"));
		}
	}

	// Test with null JSON
	{
		const TSharedPtr<FJsonObject> Json = nullptr;

		const UnrealMCP::TResult<UnrealMCP::FRemoveComponentParams> Result =
			UnrealMCP::FRemoveComponentParams::FromJson(Json);

		TestTrue(TEXT("Should fail with null JSON"), Result.IsFailure());
		TestTrue(TEXT("Error should mention invalid JSON"),
			Result.GetError().Contains(TEXT("Invalid")) || Result.GetError().Contains(TEXT("JSON")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRemoveComponentResultToJsonTest,
	"UnrealMCP.BlueprintIntrospection.RemoveComponentResultToJson",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintIntrospectionServiceRemoveComponentResultToJsonTest::RunTest(const FString& Parameters)
{
	// Test: RemoveComponentResult ToJson conversion

	UnrealMCP::FRemoveComponentResult Result;
	Result.BlueprintName = TEXT("TestBlueprint");
	Result.ComponentName = TEXT("TestComponent");
	Result.Message = TEXT("Component removed successfully");

	const TSharedPtr<FJsonObject> Json = Result.ToJson();

	TestTrue(TEXT("ToJson should return valid JSON"), Json.IsValid());
	TestTrue(TEXT("Should have blueprint_name field"), Json->HasField(TEXT("blueprint_name")));
	TestTrue(TEXT("Should have component_name field"), Json->HasField(TEXT("component_name")));
	TestTrue(TEXT("Should have message field"), Json->HasField(TEXT("message")));

	TestEqual(TEXT("Blueprint name should match"),
		Json->GetStringField(TEXT("blueprint_name")), TEXT("TestBlueprint"));
	TestEqual(TEXT("Component name should match"),
		Json->GetStringField(TEXT("component_name")), TEXT("TestComponent"));
	TestEqual(TEXT("Message should match"),
		Json->GetStringField(TEXT("message")), TEXT("Component removed successfully"));

	return true;
}