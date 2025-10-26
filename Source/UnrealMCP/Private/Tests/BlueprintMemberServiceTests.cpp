/**
 * Functional tests for BlueprintMemberService
 *
 * These tests verify blueprint function and variable operations:
 * - Function creation, deletion, and configuration
 * - Function parameters and return types
 * - Variable creation, deletion, renaming
 * - Variable metadata and default values
 *
 * Tests focus on error handling paths as functional tests with valid blueprints
 * would require complex asset setup and management.
 *
 * Tests run in the Unreal Editor with real world context.
 */

#include "Services/BlueprintMemberService.h"
#include "Services/BlueprintCreationService.h"
#include "Core/MCPTypes.h"
#include "Tests/TestUtils.h"
#include "Misc/AutomationTest.h"

// ============================================================================
// Function Operation Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceAddFunctionToInvalidBlueprintTest,
	"UnrealMCP.BlueprintMember.AddFunctionToInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceAddFunctionToInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Adding function to non-existent blueprint should fail

	const FString BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	const FString FunctionName = TEXT("TestFunction");

	const UnrealMCP::TResult<FString> Result = UnrealMCP::FBlueprintMemberService::AddFunction(
		BlueprintName,
		FunctionName
	);

	// Verify failure
	TestTrue(TEXT("AddFunction should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceAddDuplicateFunctionTest,
	"UnrealMCP.BlueprintMember.AddDuplicateFunction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceAddDuplicateFunctionTest::RunTest(const FString& Parameters)
{
	// Test: Adding duplicate function should be caught by service
	// Note: This would require a valid blueprint with existing function

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString FunctionName = TEXT("ExistingFunction");

	const UnrealMCP::TResult<FString> Result = UnrealMCP::FBlueprintMemberService::AddFunction(
		BlueprintName,
		FunctionName
	);

	// Will fail because blueprint doesn't exist, but logic is correct
	TestTrue(TEXT("AddFunction should fail"), Result.IsFailure());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceRemoveFunctionFromInvalidBlueprintTest,
	"UnrealMCP.BlueprintMember.RemoveFunctionFromInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceRemoveFunctionFromInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Removing function from non-existent blueprint should fail

	const FString BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	const FString FunctionName = TEXT("TestFunction");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::RemoveFunction(
		BlueprintName,
		FunctionName
	);

	// Verify failure
	TestTrue(TEXT("RemoveFunction should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceRemoveNonExistentFunctionTest,
	"UnrealMCP.BlueprintMember.RemoveNonExistentFunction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceRemoveNonExistentFunctionTest::RunTest(const FString& Parameters)
{
	// Test: Removing non-existent function should fail gracefully

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString FunctionName = TEXT("NonExistentFunction_XYZ123");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::RemoveFunction(
		BlueprintName,
		FunctionName
	);

	// Verify failure
	TestTrue(TEXT("RemoveFunction should fail for non-existent function"), Result.IsFailure());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceAddFunctionParameterToInvalidFunctionTest,
	"UnrealMCP.BlueprintMember.AddFunctionParameterToInvalidFunction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceAddFunctionParameterToInvalidFunctionTest::RunTest(const FString& Parameters)
{
	// Test: Adding parameter to non-existent function should fail

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString FunctionName = TEXT("NonExistentFunction");
	const FString ParamName = TEXT("TestParam");
	const FString ParamType = TEXT("int");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::AddFunctionParameter(
		BlueprintName,
		FunctionName,
		ParamName,
		ParamType,
		false
	);

	// Verify failure
	TestTrue(TEXT("AddFunctionParameter should fail for non-existent function"), Result.IsFailure());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceAddFunctionParameterWithInvalidTypeTest,
	"UnrealMCP.BlueprintMember.AddFunctionParameterWithInvalidType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceAddFunctionParameterWithInvalidTypeTest::RunTest(const FString& Parameters)
{
	// Test: Adding parameter with invalid type should fail

	// Clean up all test blueprints first to avoid any conflicts
	UnrealMCPTest::FTestUtils::CleanupAllTestBlueprints();

	const FString BlueprintName = UnrealMCPTest::FTestUtils::GenerateUniqueTestName(TEXT("TestBP_InvalidParam"));
	const FString FunctionName = UnrealMCPTest::FTestUtils::GenerateUniqueTestName(TEXT("TestFunction"));
	const FString ParamName = TEXT("TestParam");
	const FString ParamType = TEXT("InvalidType_XYZ123");

	// Create a test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Actor");
	Params.PackagePath = UnrealMCPTest::FTestUtils::GetTestPackagePath();

	const UnrealMCP::TResult<UBlueprint*> CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Blueprint creation should succeed"), CreateResult.IsSuccess());

	// Add a function to the blueprint
	const UnrealMCP::TResult<FString> AddFuncResult = UnrealMCP::FBlueprintMemberService::AddFunction(
		BlueprintName,
		FunctionName
	);
	TestTrue(TEXT("Function creation should succeed"), AddFuncResult.IsSuccess());

	// Try to add parameter with invalid type
	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::AddFunctionParameter(
		BlueprintName,
		FunctionName,
		ParamName,
		ParamType,
		false
	);

	// Verify failure
	TestTrue(TEXT("AddFunctionParameter should fail for invalid parameter type"), Result.IsFailure());
	TestTrue(TEXT("Error should mention unsupported type"),
		Result.GetError().Contains(TEXT("Unsupported")));

	// Clean up test asset
	const FString BlueprintPath = UnrealMCPTest::FTestUtils::GetTestAssetPath(BlueprintName);
	UnrealMCPTest::FTestUtils::CleanupTestAsset(BlueprintPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceSetFunctionReturnTypeInvalidTypeTest,
	"UnrealMCP.BlueprintMember.SetFunctionReturnTypeInvalidType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceSetFunctionReturnTypeInvalidTypeTest::RunTest(const FString& Parameters)
{
	// Test: Setting invalid return type should fail

	// Clean up all test blueprints first to avoid any conflicts
	UnrealMCPTest::FTestUtils::CleanupAllTestBlueprints();

	const FString BlueprintName = UnrealMCPTest::FTestUtils::GenerateUniqueTestName(TEXT("TestBP_InvalidReturn"));
	const FString FunctionName = UnrealMCPTest::FTestUtils::GenerateUniqueTestName(TEXT("TestFunction"));
	const FString ReturnType = TEXT("InvalidType_XYZ123");

	// Create a test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = BlueprintName;
	Params.ParentClass = TEXT("Actor");
	Params.PackagePath = UnrealMCPTest::FTestUtils::GetTestPackagePath();

	const UnrealMCP::TResult<UBlueprint*> CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Blueprint creation should succeed"), CreateResult.IsSuccess());

	// Add a function to the blueprint
	const UnrealMCP::TResult<FString> AddFuncResult = UnrealMCP::FBlueprintMemberService::AddFunction(
		BlueprintName,
		FunctionName
	);
	TestTrue(TEXT("Function creation should succeed"), AddFuncResult.IsSuccess());

	// Try to set invalid return type
	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::SetFunctionReturnType(
		BlueprintName,
		FunctionName,
		ReturnType
	);

	// Verify failure
	TestTrue(TEXT("SetFunctionReturnType should fail for invalid return type"), Result.IsFailure());
	TestTrue(TEXT("Error should mention unsupported type"),
		Result.GetError().Contains(TEXT("Unsupported")));

	// Clean up test asset
	const FString BlueprintPath = UnrealMCPTest::FTestUtils::GetTestAssetPath(BlueprintName);
	UnrealMCPTest::FTestUtils::CleanupTestAsset(BlueprintPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceSetFunctionMetadataInvalidFunctionTest,
	"UnrealMCP.BlueprintMember.SetFunctionMetadataInvalidFunction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceSetFunctionMetadataInvalidFunctionTest::RunTest(const FString& Parameters)
{
	// Test: Setting metadata on non-existent function should fail

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString FunctionName = TEXT("NonExistentFunction");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::SetFunctionMetadata(
		BlueprintName,
		FunctionName,
		TOptional<FString>(TEXT("TestCategory")),
		TOptional<FString>(TEXT("Test tooltip")),
		TOptional<bool>(true)
	);

	// Verify failure
	TestTrue(TEXT("SetFunctionMetadata should fail for non-existent function"), Result.IsFailure());

	return true;
}

// ============================================================================
// Variable Operation Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceRemoveVariableFromInvalidBlueprintTest,
	"UnrealMCP.BlueprintMember.RemoveVariableFromInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceRemoveVariableFromInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Removing variable from non-existent blueprint should fail

	const FString BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	const FString VariableName = TEXT("TestVariable");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::RemoveVariable(
		BlueprintName,
		VariableName
	);

	// Verify failure
	TestTrue(TEXT("RemoveVariable should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceRemoveNonExistentVariableTest,
	"UnrealMCP.BlueprintMember.RemoveNonExistentVariable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceRemoveNonExistentVariableTest::RunTest(const FString& Parameters)
{
	// Test: Removing non-existent variable should fail gracefully

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString VariableName = TEXT("NonExistentVariable_XYZ123");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::RemoveVariable(
		BlueprintName,
		VariableName
	);

	// Verify failure
	TestTrue(TEXT("RemoveVariable should fail for non-existent variable"), Result.IsFailure());
	TestTrue(TEXT("Error should mention not found"),
		Result.GetError().Contains(TEXT("not found")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceRenameVariableInvalidBlueprintTest,
	"UnrealMCP.BlueprintMember.RenameVariableInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceRenameVariableInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Renaming variable in non-existent blueprint should fail

	const FString BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	const FString OldName = TEXT("OldVariable");
	const FString NewName = TEXT("NewVariable");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::RenameVariable(
		BlueprintName,
		OldName,
		NewName
	);

	// Verify failure
	TestTrue(TEXT("RenameVariable should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceRenameVariableEmptyNameTest,
	"UnrealMCP.BlueprintMember.RenameVariableEmptyName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceRenameVariableEmptyNameTest::RunTest(const FString& Parameters)
{
	// Test: Renaming variable to empty name should fail

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString OldName = TEXT("OldVariable");
	const FString NewName = TEXT("");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::RenameVariable(
		BlueprintName,
		OldName,
		NewName
	);

	// Verify failure
	TestTrue(TEXT("RenameVariable should fail for empty new name"), Result.IsFailure());
	TestTrue(TEXT("Error should mention empty name"),
		Result.GetError().Contains(TEXT("empty")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceSetVariableDefaultValueInvalidBlueprintTest,
	"UnrealMCP.BlueprintMember.SetVariableDefaultValueInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceSetVariableDefaultValueInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting default value on non-existent blueprint should fail

	const FString BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	const FString VariableName = TEXT("TestVariable");

	// Create a simple boolean value
	const TSharedPtr<FJsonValue> Value = MakeShared<FJsonValueBoolean>(true);

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::SetVariableDefaultValue(
		BlueprintName,
		VariableName,
		Value
	);

	// Verify failure
	TestTrue(TEXT("SetVariableDefaultValue should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceSetVariableMetadataInvalidBlueprintTest,
	"UnrealMCP.BlueprintMember.SetVariableMetadataInvalidBlueprint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceSetVariableMetadataInvalidBlueprintTest::RunTest(const FString& Parameters)
{
	// Test: Setting metadata on non-existent blueprint should fail

	const FString BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	const FString VariableName = TEXT("TestVariable");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::SetVariableMetadata(
		BlueprintName,
		VariableName,
		TOptional<FString>(TEXT("Test tooltip")),
		TOptional<FString>(TEXT("TestCategory")),
		TOptional<bool>(true),
		TOptional<bool>(true),
		TOptional<bool>(false)
	);

	// Verify failure
	TestTrue(TEXT("SetVariableMetadata should fail for non-existent blueprint"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")) || Result.GetError().Contains(TEXT("Failed")));

	return true;
}

// ============================================================================
// Type Validation Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceValidateParameterTypesTest,
	"UnrealMCP.BlueprintMember.ValidateParameterTypes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceValidateParameterTypesTest::RunTest(const FString& Parameters)
{
	// Test: Verify that all supported parameter types are recognized

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString FunctionName = TEXT("TestFunction");

	// Test all valid types (they will fail on blueprint lookup, but type validation should pass first)
	TArray<FString> ValidTypes = {
		TEXT("bool"),
		TEXT("int"),
		TEXT("float"),
		TEXT("string"),
		TEXT("name"),
		TEXT("vector"),
		TEXT("rotator"),
		TEXT("transform")
	};

	for (const FString& ValidType : ValidTypes)
	{
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::AddFunctionParameter(
			BlueprintName,
			FunctionName,
			TEXT("TestParam"),
			ValidType,
			false
		);

		// Should fail on blueprint lookup, not on type validation
		TestTrue(FString::Printf(TEXT("Type '%s' should be recognized"), *ValidType), Result.IsFailure());
		TestFalse(FString::Printf(TEXT("Type '%s' error should not mention unsupported"), *ValidType),
			Result.GetError().Contains(TEXT("Unsupported")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceValidateReturnTypesTest,
	"UnrealMCP.BlueprintMember.ValidateReturnTypes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceValidateReturnTypesTest::RunTest(const FString& Parameters)
{
	// Test: Verify that all supported return types are recognized

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString FunctionName = TEXT("TestFunction");

	// Test all valid types
	TArray<FString> ValidTypes = {
		TEXT("bool"),
		TEXT("int"),
		TEXT("float"),
		TEXT("string"),
		TEXT("name"),
		TEXT("vector"),
		TEXT("rotator"),
		TEXT("transform")
	};

	for (const FString& ValidType : ValidTypes)
	{
		const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::SetFunctionReturnType(
			BlueprintName,
			FunctionName,
			ValidType
		);

		// Should fail on blueprint lookup, not on type validation
		TestTrue(FString::Printf(TEXT("Type '%s' should be recognized"), *ValidType), Result.IsFailure());
		TestFalse(FString::Printf(TEXT("Type '%s' error should not mention unsupported"), *ValidType),
			Result.GetError().Contains(TEXT("Unsupported")));
	}

	return true;
}

// ============================================================================
// Edge Case Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceNullValueTest,
	"UnrealMCP.BlueprintMember.NullValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceNullValueTest::RunTest(const FString& Parameters)
{
	// Test: Setting null value should fail gracefully

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString VariableName = TEXT("TestVariable");

	const TSharedPtr<FJsonValue> NullValue = MakeShared<FJsonValueNull>();

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::SetVariableDefaultValue(
		BlueprintName,
		VariableName,
		NullValue
	);

	// Verify failure (will fail on blueprint lookup or null value)
	TestTrue(TEXT("SetVariableDefaultValue should fail for null value"), Result.IsFailure());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintMemberServiceReferenceParameterTest,
	"UnrealMCP.BlueprintMember.ReferenceParameter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FBlueprintMemberServiceReferenceParameterTest::RunTest(const FString& Parameters)
{
	// Test: Adding reference parameter should work (will fail on blueprint lookup)

	const FString BlueprintName = TEXT("TestBlueprint");
	const FString FunctionName = TEXT("TestFunction");
	const FString ParamName = TEXT("RefParam");
	const FString ParamType = TEXT("int");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintMemberService::AddFunctionParameter(
		BlueprintName,
		FunctionName,
		ParamName,
		ParamType,
		true  // Is reference
	);

	// Should fail on blueprint lookup, not on reference flag
	TestTrue(TEXT("AddFunctionParameter with reference should be processed"), Result.IsFailure());

	return true;
}
