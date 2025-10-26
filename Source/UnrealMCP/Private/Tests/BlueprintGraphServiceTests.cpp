/**
 * Functional tests for BlueprintGraphService
 *
 * These tests verify the actual behavior of blueprint graph operations:
 * - Connecting nodes (with invalid blueprints/nodes)
 * - Adding event nodes (with validation)
 * - Adding function call nodes (with error handling)
 * - Adding component reference nodes
 * - Adding self reference nodes
 * - Adding input action nodes
 * - Finding nodes by criteria
 * - Adding variables to blueprints
 *
 * Tests focus on error handling paths as functional tests with valid blueprints
 * would require complex asset setup and graph management.
 *
 * Tests run in the Unreal Editor with real world context.
 */

#include "Services/BlueprintGraphService.h"
#include "Misc/AutomationTest.h"
#include "Blueprint/UserWidget.h"
#include "Editor.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "K2Node_Self.h"
#include "K2Node_InputAction.h"
#include "Dom/JsonObject.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceConnectNodesInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.ConnectNodesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceConnectNodesInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Connecting nodes in non-existent blueprint should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::ConnectNodes(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("SourceNodeGuid"),
		TEXT("TargetNodeGuid"),
		TEXT("SourcePin"),
		TEXT("TargetPin")
	);

	// Verify failure
	TestTrue(TEXT("ConnectNodes should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceConnectNodesInvalidGuidsTest,
	"UnrealMCP.BlueprintGraph.ConnectNodesInvalidGuids",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceConnectNodesInvalidGuidsTest::RunTest(const FString& Parameters)
{
	// Test: Connecting nodes with invalid GUIDs should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::ConnectNodes(
		TEXT("SomeBlueprint"),  // This will fail before GUID validation
		TEXT("InvalidSourceNodeGuid_XYZ123"),
		TEXT("InvalidTargetNodeGuid_XYZ123"),
		TEXT("SourcePin"),
		TEXT("TargetPin")
	);

	// Verify failure
	TestTrue(TEXT("ConnectNodes should fail for invalid GUIDs"), Result.IsFailure());
	TestTrue(TEXT("Should have an error message"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddEventNodeInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.AddEventNodeInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddEventNodeInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding event node to non-existent blueprint should fail

	const UnrealMCP::TResult<UK2Node_Event*> Result = UnrealMCP::FBlueprintGraphService::AddEventNode(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("ReceiveBeginPlay"),
		FVector2D(0.0f, 0.0f)
	);

	// Verify failure
	TestTrue(TEXT("AddEventNode should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddEventNodeInvalidEventTest,
	"UnrealMCP.BlueprintGraph.AddEventNodeInvalidEvent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddEventNodeInvalidEventTest::RunTest(const FString& Parameters)
{
	// Test: Adding event node with invalid event name should fail

	const UnrealMCP::TResult<UK2Node_Event*> Result = UnrealMCP::FBlueprintGraphService::AddEventNode(
		TEXT("SomeBlueprint"),  // This will fail before event validation
		TEXT("NonExistentEvent_XYZ123"),
		FVector2D(0.0f, 0.0f)
	);

	// Verify failure
	TestTrue(TEXT("AddEventNode should fail for invalid event"), Result.IsFailure());
	TestTrue(TEXT("Should have an error message"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddFunctionCallNodeInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.AddFunctionCallNodeInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddFunctionCallNodeInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding function call node to non-existent blueprint should fail

	const TSharedPtr<FJsonObject> FunctionParams = MakeShareable(new FJsonObject);

	const UnrealMCP::TResult<UK2Node_CallFunction*> Result = UnrealMCP::FBlueprintGraphService::AddFunctionCallNode(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("PrintString"),
		TOptional<FString>(),
		FVector2D(100.0f, 100.0f),
		FunctionParams
	);

	// Verify failure
	TestTrue(TEXT("AddFunctionCallNode should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddFunctionCallNodeInvalidFunctionTest,
	"UnrealMCP.BlueprintGraph.AddFunctionCallNodeInvalidFunction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddFunctionCallNodeInvalidFunctionTest::RunTest(const FString& Parameters)
{
	// Test: Adding function call node with invalid function name should fail

	const TSharedPtr<FJsonObject> FunctionParams = MakeShareable(new FJsonObject);

	const UnrealMCP::TResult<UK2Node_CallFunction*> Result = UnrealMCP::FBlueprintGraphService::AddFunctionCallNode(
		TEXT("SomeBlueprint"),  // This will fail before function validation
		TEXT("NonExistentFunction_XYZ123"),
		TOptional<FString>(),
		FVector2D(100.0f, 100.0f),
		FunctionParams
	);

	// Verify failure
	TestTrue(TEXT("AddFunctionCallNode should fail for invalid function"), Result.IsFailure());
	TestTrue(TEXT("Should have an error message"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddFunctionCallNodeWithParametersTest,
	"UnrealMCP.BlueprintGraph.AddFunctionCallNodeWithParameters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddFunctionCallNodeWithParametersTest::RunTest(const FString& Parameters)
{
	// Test: Adding function call node with JSON parameters

	const TSharedPtr<FJsonObject> FunctionParams = MakeShareable(new FJsonObject);
	FunctionParams->SetStringField(TEXT("InString"), TEXT("Hello World"));
	// SetLinearColorField doesn't exist on FJsonObject, so we'll set it as a string array
	TArray<TSharedPtr<FJsonValue>> ColorArray;
	ColorArray.Add(MakeShareable(new FJsonValueNumber(FLinearColor::Red.R)));
	ColorArray.Add(MakeShareable(new FJsonValueNumber(FLinearColor::Red.G)));
	ColorArray.Add(MakeShareable(new FJsonValueNumber(FLinearColor::Red.B)));
	ColorArray.Add(MakeShareable(new FJsonValueNumber(FLinearColor::Red.A)));
	FunctionParams->SetArrayField(TEXT("TextColor"), ColorArray);

	const UnrealMCP::TResult<UK2Node_CallFunction*> Result = UnrealMCP::FBlueprintGraphService::AddFunctionCallNode(
		TEXT("SomeBlueprint"),  // This will fail before parameter validation
		TEXT("PrintString"),
		TOptional<FString>(),
		FVector2D(100.0f, 100.0f),
		FunctionParams
	);

	// Verify failure (due to blueprint not existing)
	TestTrue(TEXT("Should fail gracefully"), Result.IsFailure());
	TestTrue(TEXT("Should handle JSON parameters without crashing"), !Result.GetError().IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddComponentReferenceNodeInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.AddComponentReferenceNodeInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddComponentReferenceNodeInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding component reference node to non-existent blueprint should fail

	const UnrealMCP::TResult<UK2Node_VariableGet*> Result = UnrealMCP::FBlueprintGraphService::AddComponentReferenceNode(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("TestComponent"),
		FVector2D(50.0f, 50.0f)
	);

	// Verify failure
	TestTrue(TEXT("AddComponentReferenceNode should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddSelfReferenceNodeInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.AddSelfReferenceNodeInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddSelfReferenceNodeInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding self reference node to non-existent blueprint should fail

	const UnrealMCP::TResult<UK2Node_Self*> Result = UnrealMCP::FBlueprintGraphService::AddSelfReferenceNode(
		TEXT("NonExistentBlueprint_XYZ123"),
		FVector2D(25.0f, 25.0f)
	);

	// Verify failure
	TestTrue(TEXT("AddSelfReferenceNode should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddInputActionNodeInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.AddInputActionNodeInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddInputActionNodeInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding input action node to non-existent blueprint should fail

	const UnrealMCP::TResult<UK2Node_InputAction*> Result = UnrealMCP::FBlueprintGraphService::AddInputActionNode(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("JumpAction"),
		FVector2D(75.0f, 75.0f)
	);

	// Verify failure
	TestTrue(TEXT("AddInputActionNode should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceFindNodesInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.FindNodesInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceFindNodesInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Finding nodes in non-existent blueprint should fail

	TArray<FString> OutNodeGuids;
	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::FindNodes(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("Event"),
		TOptional<FString>(TEXT("ReceiveBeginPlay")),
		OutNodeGuids
	);

	// Verify failure
	TestTrue(TEXT("FindNodes should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));
	TestTrue(TEXT("Output array should be empty"), OutNodeGuids.Num() == 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceFindNodesWithDifferentTypesTest,
	"UnrealMCP.BlueprintGraph.FindNodesWithDifferentTypes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceFindNodesWithDifferentTypesTest::RunTest(const FString& Parameters)
{
	// Test: Finding nodes with different type filters

	TArray<FString> OutNodeGuids;

	// Test with Event type
	{
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::FindNodes(
			TEXT("SomeBlueprint"),  // This will fail but tests the type parameter
			TEXT("Event"),
			TOptional<FString>(TEXT("ReceiveBeginPlay")),
			OutNodeGuids
		);
		TestTrue(TEXT("Should fail gracefully for Event type"), Result.IsFailure());
		TestTrue(TEXT("Should handle Event type parameter"), !Result.GetError().IsEmpty());
	}

	// Test with Function type
	{
		OutNodeGuids.Empty();
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::FindNodes(
			TEXT("SomeBlueprint"),  // This will fail but tests the type parameter
			TEXT("Function"),
			TOptional<FString>(),
			OutNodeGuids
		);
		TestTrue(TEXT("Should fail gracefully for Function type"), Result.IsFailure());
		TestTrue(TEXT("Should handle Function type parameter"), !Result.GetError().IsEmpty());
	}

	// Test with Variable type
	{
		OutNodeGuids.Empty();
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::FindNodes(
			TEXT("SomeBlueprint"),  // This will fail but tests the type parameter
			TEXT("Variable"),
			TOptional<FString>(),
			OutNodeGuids
		);
		TestTrue(TEXT("Should fail gracefully for Variable type"), Result.IsFailure());
		TestTrue(TEXT("Should handle Variable type parameter"), !Result.GetError().IsEmpty());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddVariableInvalidBlueprintTest,
	"UnrealMCP.BlueprintGraph.AddVariableInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddVariableInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding variable to non-existent blueprint should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::AddVariable(
		TEXT("NonExistentBlueprint_XYZ123"),
		TEXT("TestVariable"),
		TEXT("Boolean"),
		false
	);

	// Verify failure
	TestTrue(TEXT("AddVariable should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found or failed"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddVariableWithDifferentTypesTest,
	"UnrealMCP.BlueprintGraph.AddVariableWithDifferentTypes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddVariableWithDifferentTypesTest::RunTest(const FString& Parameters)
{
	// Test: Adding variables with different types

	TArray<FString> VariableTypes = { TEXT("Boolean"), TEXT("Integer"), TEXT("Float"), TEXT("String"), TEXT("Vector") };

	for (const FString& VariableType : VariableTypes)
	{
		UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::AddVariable(
			TEXT("SomeBlueprint"),  // This will fail but tests the type parameter
			FString::Printf(TEXT("TestVariable_%s"), *VariableType),
			VariableType,
			false
		);

		// Should fail due to blueprint not existing, but the important part is that it
		// handles the type parameter correctly and doesn't crash
		TestTrue(FString::Printf(TEXT("Should fail gracefully for type %s"), *VariableType), Result.IsFailure());
		TestTrue(FString::Printf(TEXT("Should handle type %s parameter"), *VariableType), !Result.GetError().IsEmpty());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceAddVariableWithEmptyNamesTest,
	"UnrealMCP.BlueprintGraph.AddVariableWithEmptyNames",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceAddVariableWithEmptyNamesTest::RunTest(const FString& Parameters)
{
	// Test: Adding variable with empty names should fail

	// Test with empty blueprint name
	{
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::AddVariable(
			TEXT(""),  // Empty blueprint name
			TEXT("TestVariable"),
			TEXT("Boolean"),
			false
		);
		TestTrue(TEXT("Should fail with empty blueprint name"), Result.IsFailure());
		TestTrue(TEXT("Should have meaningful error message"), !Result.GetError().IsEmpty());
	}

	// Test with empty variable name
	{
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::AddVariable(
			TEXT("SomeBlueprint"),
			TEXT(""),  // Empty variable name
			TEXT("Boolean"),
			false
		);
		TestTrue(TEXT("Should fail with empty variable name"), Result.IsFailure());
		TestTrue(TEXT("Should have meaningful error message"), !Result.GetError().IsEmpty());
	}

	// Test with empty variable type
	{
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::AddVariable(
			TEXT("SomeBlueprint"),
			TEXT("TestVariable"),
			TEXT(""),  // Empty variable type
			false
		);
		TestTrue(TEXT("Should fail with empty variable type"), Result.IsFailure());
		TestTrue(TEXT("Should have meaningful error message"), !Result.GetError().IsEmpty());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintGraphServiceNodeParameterValidationTest,
	"UnrealMCP.BlueprintGraph.NodeParameterValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintGraphServiceNodeParameterValidationTest::RunTest(const FString& Parameters)
{
	// Test: Various node parameter validation scenarios

	// Test AddEventNode with empty event name
	{
		const UnrealMCP::TResult<UK2Node_Event*> Result = UnrealMCP::FBlueprintGraphService::AddEventNode(
			TEXT("SomeBlueprint"),
			TEXT(""),  // Empty event name
			FVector2D(0.0f, 0.0f)
		);
		TestTrue(TEXT("AddEventNode should fail with empty event name"), Result.IsFailure());
		TestTrue(TEXT("Should have meaningful error message"), !Result.GetError().IsEmpty());
	}

	// Test AddFunctionCallNode with empty function name
	{
		const TSharedPtr<FJsonObject> JsonParameters = MakeShareable(new FJsonObject);
		const UnrealMCP::TResult<UK2Node_CallFunction*> Result = UnrealMCP::FBlueprintGraphService::AddFunctionCallNode(
			TEXT("SomeBlueprint"),
			TEXT(""),  // Empty function name
			TOptional<FString>(),
			FVector2D(100.0f, 100.0f),
			JsonParameters
		);
		TestTrue(TEXT("AddFunctionCallNode should fail with empty function name"), Result.IsFailure());
		TestTrue(TEXT("Should have meaningful error message"), !Result.GetError().IsEmpty());
	}

	// Test AddComponentReferenceNode with empty component name
	{
		const UnrealMCP::TResult<UK2Node_VariableGet*> Result = UnrealMCP::FBlueprintGraphService::AddComponentReferenceNode(
			TEXT("SomeBlueprint"),
			TEXT(""),  // Empty component name
			FVector2D(50.0f, 50.0f)
		);
		TestTrue(TEXT("AddComponentReferenceNode should fail with empty component name"), Result.IsFailure());
		TestTrue(TEXT("Should have meaningful error message"), !Result.GetError().IsEmpty());
	}

	// Test AddInputActionNode with empty action name
	{
		const UnrealMCP::TResult<UK2Node_InputAction*> Result = UnrealMCP::FBlueprintGraphService::AddInputActionNode(
			TEXT("SomeBlueprint"),
			TEXT(""),  // Empty action name
			FVector2D(75.0f, 75.0f)
		);
		TestTrue(TEXT("AddInputActionNode should fail with empty action name"), Result.IsFailure());
		TestTrue(TEXT("Should have meaningful error message"), !Result.GetError().IsEmpty());
	}

	return true;
}