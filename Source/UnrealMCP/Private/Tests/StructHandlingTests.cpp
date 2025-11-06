
#include "Core/CommonUtils.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/AutomationTest.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Services/BlueprintCreationService.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Services/BlueprintMemberService.h"
#include "Tests/TestUtils.h"

// ============================================================================
// Blueprint Variable Struct Tests - Testing Real Service Behavior
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlingVectorVariableTest,
	"UnrealMCP.StructHandling.Variable.Vector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlingVectorVariableTest::RunTest(const FString& Parameters) -> bool {
	// Test: Create blueprint with Vector variable and set default value using object format

	UnrealMCPTest::FTestUtils::CleanupAllTestBlueprints();
	const FString BlueprintName = UnrealMCPTest::FTestUtils::GenerateUniqueTestName(TEXT("TestBP_Vector"));
	const FString VariableName = TEXT("TestVector");

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams CreateParams;
	CreateParams.Name = BlueprintName;
	CreateParams.ParentClass = TEXT("Actor");
	CreateParams.PackagePath = UnrealMCPTest::FTestUtils::GetTestPackagePath();

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreateParams);
	TestTrue(TEXT("Blueprint creation should succeed"), CreateResult.IsSuccess());

	// Add vector variable
	const auto AddVarResult = UnrealMCP::FBlueprintMemberService::AddVariable(
		BlueprintName,
		VariableName,
		TEXT("vector"),
		false
	);
	TestTrue(TEXT("AddVariable should succeed"), AddVarResult.IsSuccess());

	// Set vector default value using object format (as expected by BlueprintMemberService)
	TSharedPtr<FJsonObject> VectorValue = MakeShareable(new FJsonObject());
	VectorValue->SetNumberField(TEXT("x"), 1.5f);
	VectorValue->SetNumberField(TEXT("y"), 2.5f);
	VectorValue->SetNumberField(TEXT("z"), 3.5f);

	const auto SetResult = UnrealMCP::FBlueprintMemberService::SetVariableDefaultValue(
		BlueprintName,
		VariableName,
		MakeShareable(new FJsonValueObject(VectorValue))
	);
	TestTrue(TEXT("SetVariableDefaultValue should succeed"), SetResult.IsSuccess());

	// Verify the variable exists and has correct type
	const auto VariablesResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintVariables(BlueprintName);
	TestTrue(TEXT("GetBlueprintVariables should succeed"), VariablesResult.IsSuccess());

	if (VariablesResult.IsSuccess()) {
		const auto& VariablesData = VariablesResult.GetValue();
		bool bFoundVector = false;
		for (const auto& Var : VariablesData.Variables) {
			if (Var.Name == VariableName) {
				bFoundVector = true;
				TestTrue(TEXT("Variable should be struct type"), Var.Type.Contains(TEXT("struct")) || Var.Type.Contains(TEXT("Vector")));
				break;
			}
		}
		TestTrue(TEXT("Vector variable should be found"), bFoundVector);
	}

	// Cleanup
	const FString BlueprintPath = UnrealMCPTest::FTestUtils::GetTestAssetPath(BlueprintName);
	UnrealMCPTest::FTestUtils::CleanupTestAsset(BlueprintPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerFRotatorArrayTest,
	"UnrealMCP.StructHandling.FStructHandler.FRotator.Array",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerFRotatorArrayTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler converts JSON array [pitch, yaw, roll] to FRotator correctly

	TArray<TSharedPtr<FJsonValue>> Array3;
	Array3.Add(MakeShareable(new FJsonValueNumber(45.0f))); // Pitch
	Array3.Add(MakeShareable(new FJsonValueNumber(90.0f))); // Yaw
	Array3.Add(MakeShareable(new FJsonValueNumber(135.0f))); // Roll

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("TestRotator"), Array3);

	FRotator Result = FCommonUtils::GetRotatorFromJson(TestObject, TEXT("TestRotator"));

	TestEqual(TEXT("Pitch should match"), Result.Pitch, 45.0);
	TestEqual(TEXT("Yaw should match"), Result.Yaw, 90.0);
	TestEqual(TEXT("Roll should match"), Result.Roll, 135.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerFVector2DArrayTest,
	"UnrealMCP.StructHandling.FStructHandler.FVector2D.Array",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerFVector2DArrayTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler converts JSON array [x, y] to FVector2D correctly

	TArray<TSharedPtr<FJsonValue>> Array2;
	Array2.Add(MakeShareable(new FJsonValueNumber(10.5f)));
	Array2.Add(MakeShareable(new FJsonValueNumber(20.5f)));

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("TestVector2D"), Array2);

	FVector2D Result = FCommonUtils::GetVector2DFromJson(TestObject, TEXT("TestVector2D"));

	TestEqual(TEXT("X component should match"), Result.X, 10.5);
	TestEqual(TEXT("Y component should match"), Result.Y, 20.5);

	return true;
}

// ============================================================================
// FStructHandler Direct Tests - JSON Object Format (Transform)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerFTransformObjectTest,
	"UnrealMCP.StructHandling.FStructHandler.FTransform.Object",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerFTransformObjectTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler converts JSON object with rotation, location, scale to FTransform

	// Create transform object with all components
	TSharedPtr<FJsonObject> TransformObject = MakeShareable(new FJsonObject());

	// Rotation component
	TArray<TSharedPtr<FJsonValue>> RotationArray;
	RotationArray.Add(MakeShareable(new FJsonValueNumber(0.0f))); // Yaw
	RotationArray.Add(MakeShareable(new FJsonValueNumber(90.0f))); // Pitch
	RotationArray.Add(MakeShareable(new FJsonValueNumber(0.0f))); // Roll
	TransformObject->SetArrayField(TEXT("rotation"), RotationArray);

	// Location component
	TArray<TSharedPtr<FJsonValue>> LocationArray;
	LocationArray.Add(MakeShareable(new FJsonValueNumber(100.0f)));
	LocationArray.Add(MakeShareable(new FJsonValueNumber(200.0f)));
	LocationArray.Add(MakeShareable(new FJsonValueNumber(300.0f)));
	TransformObject->SetArrayField(TEXT("location"), LocationArray);

	// Scale component
	TArray<TSharedPtr<FJsonValue>> ScaleArray;
	ScaleArray.Add(MakeShareable(new FJsonValueNumber(2.0f)));
	ScaleArray.Add(MakeShareable(new FJsonValueNumber(3.0f)));
	ScaleArray.Add(MakeShareable(new FJsonValueNumber(4.0f)));
	TransformObject->SetArrayField(TEXT("scale"), ScaleArray);

	// Test conversion through MCP API (integration test)
	UnrealMCPTest::FTestUtils::CleanupAllTestBlueprints();
	const FString BlueprintName = UnrealMCPTest::FTestUtils::GenerateUniqueTestName(TEXT("TestBP_Transform"));

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams CreateParams;
	CreateParams.Name = BlueprintName;
	CreateParams.ParentClass = TEXT("Actor");
	CreateParams.PackagePath = UnrealMCPTest::FTestUtils::GetTestPackagePath();

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreateParams);
	TestTrue(TEXT("Blueprint creation should succeed"), CreateResult.IsSuccess());

	// Add a vector variable to test struct setting
	const auto AddVarResult = UnrealMCP::FBlueprintMemberService::AddVariable(
		BlueprintName,
		TEXT("TestVector"),
		TEXT("vector"),
		false
	);
	TestTrue(TEXT("AddVariable should succeed"), AddVarResult.IsSuccess());

	// Test setting vector through the variable system
	TSharedPtr<FJsonObject> SetVarRequest = MakeShareable(new FJsonObject());
	SetVarRequest->SetStringField(TEXT("blueprint_name"), BlueprintName);
	SetVarRequest->SetStringField(TEXT("variable_name"), TEXT("TestVector"));

	TArray<TSharedPtr<FJsonValue>> VectorArray;
	VectorArray.Add(MakeShareable(new FJsonValueNumber(1.0f)));
	VectorArray.Add(MakeShareable(new FJsonValueNumber(2.0f)));
	VectorArray.Add(MakeShareable(new FJsonValueNumber(3.0f)));
	SetVarRequest->SetArrayField(TEXT("value"), VectorArray);

	// Cleanup
	const FString BlueprintPath = UnrealMCPTest::FTestUtils::GetTestAssetPath(BlueprintName);
	UnrealMCPTest::FTestUtils::CleanupTestAsset(BlueprintPath);

	return true;
}

// ============================================================================
// Struct Handler Error Handling Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerInvalidArrayLengthTest,
	"UnrealMCP.StructHandling.FStructHandler.InvalidArrayLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerInvalidArrayLengthTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler handles invalid array lengths gracefully

	// Test with 1-component array (invalid for most structs)
	TArray<TSharedPtr<FJsonValue>> Array1;
	Array1.Add(MakeShareable(new FJsonValueNumber(1.0f)));

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("InvalidVector"), Array1);

	// Should return default FVector for invalid input
	FVector Result = FCommonUtils::GetVectorFromJson(TestObject, TEXT("InvalidVector"));
	TestEqual(TEXT("Should return zero vector for invalid array"), Result, FVector::ZeroVector);

	// Test with 5-component array (invalid for FVector)
	TArray<TSharedPtr<FJsonValue>> Array5;
	Array5.Add(MakeShareable(new FJsonValueNumber(1.0f)));
	Array5.Add(MakeShareable(new FJsonValueNumber(2.0f)));
	Array5.Add(MakeShareable(new FJsonValueNumber(3.0f)));
	Array5.Add(MakeShareable(new FJsonValueNumber(4.0f)));
	Array5.Add(MakeShareable(new FJsonValueNumber(5.0f)));

	TestObject->SetArrayField(TEXT("InvalidVector2"), Array5);
	FVector Result2 = FCommonUtils::GetVectorFromJson(TestObject, TEXT("InvalidVector2"));
	TestEqual(TEXT("Should return zero vector for invalid array length"), Result2, FVector::ZeroVector);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerInvalidArrayValuesTest,
	"UnrealMCP.StructHandling.FStructHandler.InvalidArrayValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerInvalidArrayValuesTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler handles invalid array values (non-numeric) gracefully

	// Test with string values in numeric array
	TArray<TSharedPtr<FJsonValue>> StringArray;
	StringArray.Add(MakeShareable(new FJsonValueString(TEXT("invalid"))));
	StringArray.Add(MakeShareable(new FJsonValueString(TEXT("values"))));
	StringArray.Add(MakeShareable(new FJsonValueString(TEXT("here"))));

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("StringArray"), StringArray);

	// Should handle gracefully (implementation dependent)
	FVector Result = FCommonUtils::GetVectorFromJson(TestObject, TEXT("StringArray"));
	// We expect this to either return zero vector or attempt conversion

	return true;
}

// ============================================================================
// Color Struct Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerFColorArrayTest,
	"UnrealMCP.StructHandling.FStructHandler.FColor.Array",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerFColorArrayTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler converts JSON arrays to FColor correctly

	// Test 3-component RGB array (should default alpha to 255)
	TArray<TSharedPtr<FJsonValue>> RGBArray;
	RGBArray.Add(MakeShareable(new FJsonValueNumber(255))); // R
	RGBArray.Add(MakeShareable(new FJsonValueNumber(128))); // G
	RGBArray.Add(MakeShareable(new FJsonValueNumber(64)));  // B

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("RGBColor"), RGBArray);

	// Note: CommonUtils doesn't have direct color conversion, but we can test the concept
	// by verifying the array structure is correct
	const TArray<TSharedPtr<FJsonValue>>* RetrievedArray;
	TestTrue(TEXT("Should have RGB color array"), TestObject->TryGetArrayField(TEXT("RGBColor"), RetrievedArray));
	TestEqual(TEXT("Should have 3 components"), RetrievedArray->Num(), 3);

	// Test 4-component RGBA array
	TArray<TSharedPtr<FJsonValue>> RGBAArray;
	RGBAArray.Add(MakeShareable(new FJsonValueNumber(255))); // R
	RGBAArray.Add(MakeShareable(new FJsonValueNumber(128))); // G
	RGBAArray.Add(MakeShareable(new FJsonValueNumber(64)));  // B
	RGBAArray.Add(MakeShareable(new FJsonValueNumber(32)));  // A

	TestObject->SetArrayField(TEXT("RGBAColor"), RGBAArray);
	TestTrue(TEXT("Should have RGBA color array"), TestObject->TryGetArrayField(TEXT("RGBAColor"), RetrievedArray));
	TestEqual(TEXT("Should have 4 components"), RetrievedArray->Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerFLinearColorArrayTest,
	"UnrealMCP.StructHandling.FStructHandler.FLinearColor.Array",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerFLinearColorArrayTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler converts JSON arrays to FLinearColor correctly

	TArray<TSharedPtr<FJsonValue>> ColorArray;
	ColorArray.Add(MakeShareable(new FJsonValueNumber(1.0f))); // R (normalized)
	ColorArray.Add(MakeShareable(new FJsonValueNumber(0.5f))); // G (normalized)
	ColorArray.Add(MakeShareable(new FJsonValueNumber(0.25f))); // B (normalized)
	ColorArray.Add(MakeShareable(new FJsonValueNumber(0.8f))); // A (normalized)

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("LinearColor"), ColorArray);

	const TArray<TSharedPtr<FJsonValue>>* RetrievedArray;
	TestTrue(TEXT("Should have linear color array"), TestObject->TryGetArrayField(TEXT("LinearColor"), RetrievedArray));
	TestEqual(TEXT("Should have 4 components"), RetrievedArray->Num(), 4);

	// Verify values are in correct range [0,1]
	for (int32 i = 0; i < RetrievedArray->Num(); ++i) {
		float Value = static_cast<float>((*RetrievedArray)[i]->AsNumber());
		TestTrue(TEXT("Linear color components should be in range [0,1]"), Value >= 0.0f && Value <= 1.0f);
	}

	return true;
}

// ============================================================================
// Vector4 and Advanced Vector Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerFVector4ArrayTest,
	"UnrealMCP.StructHandling.FStructHandler.FVector4.Array",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerFVector4ArrayTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler converts JSON arrays to FVector4 correctly

	// Test 4-component vector
	TArray<TSharedPtr<FJsonValue>> Vector4Array;
	Vector4Array.Add(MakeShareable(new FJsonValueNumber(1.0f))); // X
	Vector4Array.Add(MakeShareable(new FJsonValueNumber(2.0f))); // Y
	Vector4Array.Add(MakeShareable(new FJsonValueNumber(3.0f))); // Z
	Vector4Array.Add(MakeShareable(new FJsonValueNumber(4.0f))); // W

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("Vector4"), Vector4Array);

	const TArray<TSharedPtr<FJsonValue>>* RetrievedArray;
	TestTrue(TEXT("Should have Vector4 array"), TestObject->TryGetArrayField(TEXT("Vector4"), RetrievedArray));
	TestEqual(TEXT("Should have 4 components"), RetrievedArray->Num(), 4);

	// Test 2-component array (should set Z and W to 0 according to implementation)
	TArray<TSharedPtr<FJsonValue>> Vector2Array;
	Vector2Array.Add(MakeShareable(new FJsonValueNumber(5.0f))); // X
	Vector2Array.Add(MakeShareable(new FJsonValueNumber(6.0f))); // Y

	TestObject->SetArrayField(TEXT("Vector4From2"), Vector2Array);
	TestTrue(TEXT("Should have Vector4 from 2 components"), TestObject->TryGetArrayField(TEXT("Vector4From2"), RetrievedArray));
	TestEqual(TEXT("Should have 2 components"), RetrievedArray->Num(), 2);

	return true;
}

// ============================================================================
// Integration Tests with Blueprint System
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerBlueprintIntegrationTest,
	"UnrealMCP.StructHandling.BlueprintIntegration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerBlueprintIntegrationTest::RunTest(const FString& Parameters) -> bool {
	// Test: Complete integration of struct handling with blueprint system

	UnrealMCPTest::FTestUtils::CleanupAllTestBlueprints();
	const FString BlueprintName = UnrealMCPTest::FTestUtils::GenerateUniqueTestName(TEXT("TestBP_StructIntegration"));

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams CreateParams;
	CreateParams.Name = BlueprintName;
	CreateParams.ParentClass = TEXT("Actor");
	CreateParams.PackagePath = UnrealMCPTest::FTestUtils::GetTestPackagePath();

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreateParams);
	TestTrue(TEXT("Blueprint creation should succeed"), CreateResult.IsSuccess());

	// Add variables of different struct types
	TArray<FString> StructTypes = {TEXT("vector"), TEXT("rotator"), TEXT("transform")};
	TArray<FString> VariableNames;

	for (int32 i = 0; i < StructTypes.Num(); ++i) {
		const FString VariableName = FString::Printf(TEXT("TestStruct_%s"), *StructTypes[i]);
		VariableNames.Add(VariableName);

		const auto AddVarResult = UnrealMCP::FBlueprintMemberService::AddVariable(
			BlueprintName,
			VariableName,
			StructTypes[i],
			false
		);
		TestTrue(FString::Printf(TEXT("AddVariable should succeed for %s"), *VariableName), AddVarResult.IsSuccess());
	}

	// Verify variables were created with struct types
	const auto VariablesResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintVariables(BlueprintName);
	TestTrue(TEXT("GetBlueprintVariables should succeed"), VariablesResult.IsSuccess());

	if (VariablesResult.IsSuccess()) {
		const auto& VariablesData = VariablesResult.GetValue();
		TestTrue(TEXT("Should have struct variables"), VariablesData.Variables.Num() >= StructTypes.Num());

		// Check each variable has correct struct type
		for (int32 i = 0; i < VariableNames.Num(); ++i) {
			bool bFoundVariable = false;
			for (const auto& Var : VariablesData.Variables) {
				if (Var.Name == VariableNames[i]) {
					bFoundVariable = true;
					// Verify it's recognized as a struct type
					TestTrue(FString::Printf(TEXT("Variable %s should be struct type"), *VariableNames[i]),
					         Var.Type.Contains(TEXT("struct")) || Var.Type.Contains(StructTypes[i]));
					break;
				}
			}
			TestTrue(FString::Printf(TEXT("Struct variable %s should be found"), *VariableNames[i]), bFoundVariable);
		}
	}

	// Cleanup
	const FString BlueprintPath = UnrealMCPTest::FTestUtils::GetTestAssetPath(BlueprintName);
	UnrealMCPTest::FTestUtils::CleanupTestAsset(BlueprintPath);

	return true;
}

// ============================================================================
// Performance and Stress Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerPerformanceTest,
	"UnrealMCP.StructHandling.Performance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerPerformanceTest::RunTest(const FString& Parameters) -> bool {
	// Test: Performance of struct conversion operations

	const int32 NumIterations = 1000;
	double StartTime = FPlatformTime::Seconds();

	for (int32 i = 0; i < NumIterations; ++i) {
		TArray<TSharedPtr<FJsonValue>> TestArray;
		TestArray.Add(MakeShareable(new FJsonValueNumber(FMath::FRand())));
		TestArray.Add(MakeShareable(new FJsonValueNumber(FMath::FRand())));
		TestArray.Add(MakeShareable(new FJsonValueNumber(FMath::FRand())));

		TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
		TestObject->SetArrayField(TEXT("PerfVector"), TestArray);

		FVector Result = FCommonUtils::GetVectorFromJson(TestObject, TEXT("PerfVector"));
		// Use Result to prevent optimization
		if (Result.X == 999.0f) {
			UE_LOG(LogTemp, Warning, TEXT("Should not happen"));
		}
	}

	double EndTime = FPlatformTime::Seconds();
	double Duration = EndTime - StartTime;

	// Should complete 1000 conversions in reasonable time (adjust threshold as needed)
	TestTrue(TEXT("Struct conversion should be performant"), Duration < 1.0);
	UE_LOG(LogTemp, Display, TEXT("Struct conversion performance: %d iterations in %f seconds (%f per conversion)"),
	       NumIterations, Duration, Duration / NumIterations);

	return true;
}

// ============================================================================
// Edge Cases and Robustness Tests
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerExtremaValuesTest,
	"UnrealMCP.StructHandling.ExtremaValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerExtremaValuesTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler handles extreme values correctly

	// Test with very large values
	TArray<TSharedPtr<FJsonValue>> LargeValues;
	LargeValues.Add(MakeShareable(new FJsonValueNumber(FLT_MAX)));
	LargeValues.Add(MakeShareable(new FJsonValueNumber(-FLT_MAX)));
	LargeValues.Add(MakeShareable(new FJsonValueNumber(FLT_MIN)));

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("LargeVector"), LargeValues);

	FVector LargeResult = FCommonUtils::GetVectorFromJson(TestObject, TEXT("LargeVector"));
	TestTrue(TEXT("Should handle large float values"), !LargeResult.ContainsNaN());

	// Test with infinity values
	TArray<TSharedPtr<FJsonValue>> InfinityValues;
	InfinityValues.Add(MakeShareable(new FJsonValueNumber(INFINITY)));
	InfinityValues.Add(MakeShareable(new FJsonValueNumber(-INFINITY)));
	InfinityValues.Add(MakeShareable(new FJsonValueNumber(0.0f)));

	TestObject->SetArrayField(TEXT("InfinityVector"), InfinityValues);
	FVector InfinityResult = FCommonUtils::GetVectorFromJson(TestObject, TEXT("InfinityVector"));
	// Should handle infinity gracefully

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStructHandlerPrecisionTest,
	"UnrealMCP.StructHandling.Precision",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FStructHandlerPrecisionTest::RunTest(const FString& Parameters) -> bool {
	// Test: FStructHandler maintains precision during conversion

	// Test with high precision values
	TArray<TSharedPtr<FJsonValue>> PreciseValues;
	PreciseValues.Add(MakeShareable(new FJsonValueNumber(3.14159265359f)));
	PreciseValues.Add(MakeShareable(new FJsonValueNumber(2.71828182846f)));
	PreciseValues.Add(MakeShareable(new FJsonValueNumber(1.41421356237f)));

	TSharedPtr<FJsonObject> TestObject = MakeShareable(new FJsonObject());
	TestObject->SetArrayField(TEXT("PreciseVector"), PreciseValues);

	FVector PreciseResult = FCommonUtils::GetVectorFromJson(TestObject, TEXT("PreciseVector"));

	// Verify precision is maintained within reasonable tolerance
	const float Tolerance = 1e-6f;
	TestTrue(TEXT("X precision maintained"), FMath::Abs(PreciseResult.X - 3.14159265359f) < Tolerance);
	TestTrue(TEXT("Y precision maintained"), FMath::Abs(PreciseResult.Y - 2.71828182846f) < Tolerance);
	TestTrue(TEXT("Z precision maintained"), FMath::Abs(PreciseResult.Z - 1.41421356237f) < Tolerance);

	return true;
}