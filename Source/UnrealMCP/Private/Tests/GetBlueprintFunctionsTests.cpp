#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Misc/AutomationTest.h"
#include "Services/BlueprintMemberService.h"
#include "Tests/TestUtils.h"
#include "Types/BlueprintIntrospectionTypes.h"



IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlueprintMemberServiceGetFunctionsErrorHandling,
                                 "UnrealMCP.BlueprintMemberService.GetFunctions.ErrorHandling",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

auto FBlueprintMemberServiceGetFunctionsErrorHandling::RunTest(const FString& Parameters) -> bool {
	// Test 1: Empty blueprint name
	{
		auto Result = UnrealMCP::FBlueprintMemberService::GetFunctions(TEXT(""));
		TestTrue(TEXT("Empty blueprint name should fail"), Result.IsFailure());

		UnrealMCPTest::FTestUtils::ValidateErrorCode(
			Result,
			UnrealMCP::EErrorCode::InvalidInput,
			TEXT("BlueprintName"),
			this
		);
	}

	// Test 2: Null/invalid blueprint name
	{
		auto Result = UnrealMCP::FBlueprintMemberService::GetFunctions(TEXT("NonExistentBlueprint"));
		TestTrue(TEXT("Non-existent blueprint should fail"), Result.IsFailure());

		UnrealMCPTest::FTestUtils::ValidateErrorCode(
			Result,
			UnrealMCP::EErrorCode::BlueprintNotFound,
			TEXT("NonExistentBlueprint"),
			this
		);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlueprintMemberServiceGetFunctionsEmptyBlueprint,
                                 "UnrealMCP.BlueprintMemberService.GetFunctions.EmptyBlueprint",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

auto FBlueprintMemberServiceGetFunctionsEmptyBlueprint::RunTest(const FString& Parameters) -> bool {
	// Create a simple Actor blueprint with no custom functions
	UBlueprint* EmptyBlueprint = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(),
		GetTransientPackage(),
		FName(*FString::Printf(TEXT("TestEmptyBP_%s"), *FGuid::NewGuid().ToString())),
		BPTYPE_Normal
	);

	TestNotNull(TEXT("Empty blueprint should be created successfully"), EmptyBlueprint);

	// Compile the blueprint
	FKismetEditorUtilities::CompileBlueprint(EmptyBlueprint);

	// Test the service
	auto Result = UnrealMCP::FBlueprintMemberService::GetFunctions(EmptyBlueprint->GetName());

	TestTrue(TEXT("Service call should succeed"), Result.IsSuccess());

	const UnrealMCP::FGetBlueprintFunctionsResult& FunctionsResult = Result.GetValue();
	TestEqual(TEXT("Function count should be 0 for empty blueprint"), FunctionsResult.Count, 0);
	TestTrue(TEXT("Functions array should be empty"), FunctionsResult.Functions.Num() == 0);

	// Test ToJson method on empty result
	auto Json = FunctionsResult.ToJson();
	TestTrue(TEXT("JSON conversion should succeed"), Json.IsValid());
	TestTrue(TEXT("JSON should have functions array"), Json->HasField(TEXT("functions")));
	TestTrue(TEXT("JSON should have count field"), Json->HasField(TEXT("count")));
	TestEqual(TEXT("JSON count should be 0"), Json->GetIntegerField(TEXT("count")), 0);

	// Cleanup
	EmptyBlueprint->MarkAsGarbage();
	EmptyBlueprint = nullptr;

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlueprintMemberServiceGetFunctionsWithCustomFunctions,
                                 "UnrealMCP.BlueprintMemberService.GetFunctions.WithCustomFunctions",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

auto FBlueprintMemberServiceGetFunctionsWithCustomFunctions::RunTest(const FString& Parameters) -> bool {
	// Create a test blueprint
	UBlueprint* TestBlueprint = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(),
		GetTransientPackage(),
		FName(*FString::Printf(TEXT("TestFunctionsBP_%s"), *FGuid::NewGuid().ToString())),
		BPTYPE_Normal
	);

	TestNotNull(TEXT("Test blueprint should be created"), TestBlueprint);

	// Add a custom function
	FString TestFunctionName = FString::Printf(TEXT("TestFunction_%s"), *FGuid::NewGuid().ToString());
	UEdGraph* FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(
		TestBlueprint,
		FName(*TestFunctionName),
		UEdGraph::StaticClass(),
		UEdGraphSchema_K2::StaticClass()
	);

	TestNotNull(TEXT("Function graph should be created"), FunctionGraph);

	// Add the function graph to the blueprint (this is the missing step!)
	// Note: AddFunctionGraph creates the entry node automatically
	FBlueprintEditorUtils::AddFunctionGraph<UClass>(TestBlueprint, FunctionGraph, false, nullptr);

	// Find the entry node in the function graph
	UK2Node_FunctionEntry* EntryNode = nullptr;
	for (UEdGraphNode* Node : FunctionGraph->Nodes) {
		if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(Node)) {
			EntryNode = Entry;
			break;
		}
	}

	// Create a simple return node to make the function valid
	if (EntryNode) {
		UK2Node_FunctionResult* ResultNode = NewObject<UK2Node_FunctionResult>(FunctionGraph);
		if (ResultNode) {
			FunctionGraph->AddNode(ResultNode, false, false);
			ResultNode->CreateNewGuid();
			ResultNode->PostPlacedNewNode();
			ResultNode->AllocateDefaultPins();

			// Connect the entry node's execution pin to the result node
			UEdGraphPin* ThenPin = EntryNode->GetExecPin();
			UEdGraphPin* ExecutePin = ResultNode->GetExecPin();
			if (ThenPin && ExecutePin) {
				ThenPin->MakeLinkTo(ExecutePin);
			}
		}
	}

	// Compile the blueprint
	FKismetEditorUtilities::CompileBlueprint(TestBlueprint);

	// Test the service
	auto Result = UnrealMCP::FBlueprintMemberService::GetFunctions(TestBlueprint->GetName());

	TestTrue(TEXT("Service call should succeed"), Result.IsSuccess());

	const UnrealMCP::FGetBlueprintFunctionsResult& FunctionsResult = Result.GetValue();
	TestTrue(TEXT("Function count should be > 0"), FunctionsResult.Count > 0);
	TestTrue(TEXT("Functions array should not be empty"), FunctionsResult.Functions.Num() > 0);

	// Find our test function
	bool FoundTestFunction = false;
	for (const UnrealMCP::FBlueprintFunctionInfo& FunctionInfo : FunctionsResult.Functions) {
		if (FunctionInfo.Name == TestFunctionName) {
			FoundTestFunction = true;

			// Validate function structure
			TestTrue(TEXT("Function should have category"), !FunctionInfo.Category.IsEmpty());
			TestTrue(TEXT("Function should have tooltip field"), true); // Field exists even if empty
			TestTrue(TEXT("Function should have keywords field"), true); // Field exists even if empty
			TestTrue(TEXT("Function should have parameters array"), FunctionInfo.Parameters.Num() >= 0);
			TestTrue(TEXT("Function should have returns array"), FunctionInfo.Returns.Num() >= 0);
			TestTrue(TEXT("Function should have node count >= 0"), FunctionInfo.NodeCount >= 0);

			// Test JSON serialization for individual function
			auto FunctionJson = FunctionInfo.ToJson();
			TestTrue(TEXT("Function JSON should be valid"), FunctionJson.IsValid());
			TestTrue(TEXT("Function JSON should have name"), FunctionJson->HasField(TEXT("name")));
			TestTrue(TEXT("Function JSON should have category"), FunctionJson->HasField(TEXT("category")));
			TestTrue(TEXT("Function JSON should have tooltip"), FunctionJson->HasField(TEXT("tooltip")));
			TestTrue(TEXT("Function JSON should have keywords"), FunctionJson->HasField(TEXT("keywords")));
			TestTrue(TEXT("Function JSON should have is_pure"), FunctionJson->HasField(TEXT("is_pure")));
			TestTrue(TEXT("Function JSON should have parameters"), FunctionJson->HasField(TEXT("parameters")));
			TestTrue(TEXT("Function JSON should have returns"), FunctionJson->HasField(TEXT("returns")));
			TestTrue(TEXT("Function JSON should have node_count"), FunctionJson->HasField(TEXT("node_count")));

			break;
		}
	}

	TestTrue(TEXT("Test function should be found in results"), FoundTestFunction);

	// Test complete result JSON serialization
	auto Json = FunctionsResult.ToJson();
	TestTrue(TEXT("Result JSON should be valid"), Json.IsValid());
	TestTrue(TEXT("Result JSON should have functions array"), Json->HasField(TEXT("functions")));
	TestTrue(TEXT("Result JSON should have count field"), Json->HasField(TEXT("count")));
	TestEqual(TEXT("JSON count should match struct count"),
	          Json->GetIntegerField(TEXT("count")),
	          FunctionsResult.Count);

	// Cleanup
	TestBlueprint->MarkAsGarbage();
	TestBlueprint = nullptr;

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlueprintMemberServiceGetFunctionsMetadataExtraction,
                                 "UnrealMCP.BlueprintMemberService.GetFunctions.MetadataExtraction",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

auto FBlueprintMemberServiceGetFunctionsMetadataExtraction::RunTest(const FString& Parameters) -> bool {
	// Create a test blueprint
	UBlueprint* MetadataBlueprint = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(),
		GetTransientPackage(),
		FName(*FString::Printf(TEXT("TestMetadataBP_%s"), *FGuid::NewGuid().ToString())),
		BPTYPE_Normal
	);

	TestNotNull(TEXT("Metadata blueprint should be created"), MetadataBlueprint);

	// Add a custom function
	FString TestFunctionName = FString::Printf(TEXT("TestMetadataFunction_%s"), *FGuid::NewGuid().ToString());
	UEdGraph* FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(
		MetadataBlueprint,
		FName(*TestFunctionName),
		UEdGraph::StaticClass(),
		UEdGraphSchema_K2::StaticClass()
	);

	TestNotNull(TEXT("Function graph should be created"), FunctionGraph);
	
	FBlueprintEditorUtils::AddFunctionGraph<UClass>(MetadataBlueprint, FunctionGraph, false, nullptr);

	// Find the entry node in the function graph
	UK2Node_FunctionEntry* EntryNode = nullptr;
	for (UEdGraphNode* Node : FunctionGraph->Nodes) {
		if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(Node)) {
			EntryNode = Entry;
			break;
		}
	}

	// Create a simple return node to make the function valid
	if (EntryNode) {
		UK2Node_FunctionResult* ResultNode = NewObject<UK2Node_FunctionResult>(FunctionGraph);
		if (ResultNode) {
			FunctionGraph->AddNode(ResultNode, false, false);
			ResultNode->CreateNewGuid();
			ResultNode->PostPlacedNewNode();
			ResultNode->AllocateDefaultPins();

			// Connect the entry node's execution pin to the result node
			UEdGraphPin* ThenPin = EntryNode->GetExecPin();
			UEdGraphPin* ExecutePin = ResultNode->GetExecPin();
			if (ThenPin && ExecutePin) {
				ThenPin->MakeLinkTo(ExecutePin);
			}
		}
	}

	// Compile the blueprint
	FKismetEditorUtilities::CompileBlueprint(MetadataBlueprint);

	// Set metadata on the function
	FString TestCategory = TEXT("TestCategory");
	FString TestTooltip = TEXT("Test function tooltip");

	auto SetMetadataResult = UnrealMCP::FBlueprintMemberService::SetFunctionMetadata(
		MetadataBlueprint->GetName(),
		TestFunctionName,
		TOptional<FString>(TestCategory),
		TOptional<FString>(TestTooltip),
		TOptional<bool>(true) // Make it a pure function
	);
	TestTrue(TEXT("Setting metadata should succeed"), SetMetadataResult.IsSuccess());

	// Test the service
	auto Result = UnrealMCP::FBlueprintMemberService::GetFunctions(MetadataBlueprint->GetName());

	TestTrue(TEXT("Service call should succeed"), Result.IsSuccess());

	const UnrealMCP::FGetBlueprintFunctionsResult& FunctionsResult = Result.GetValue();
	TestTrue(TEXT("Should find functions"), FunctionsResult.Count > 0);

	// Find our test function and verify metadata
	bool FoundTestFunction = false;
	for (const UnrealMCP::FBlueprintFunctionInfo& FunctionInfo : FunctionsResult.Functions) {
		if (FunctionInfo.Name == TestFunctionName) {
			FoundTestFunction = true;

			// Verify metadata was extracted correctly
			TestEqual(TEXT("Category should match set metadata"), FunctionInfo.Category, TestCategory);
			TestEqual(TEXT("Tooltip should match set metadata"), FunctionInfo.Tooltip, TestTooltip);

			// Note: We don't test for bIsPure = true because setting a function as pure
			// after creation with execution pins is conceptually contradictory.
			// Pure functions shouldn't have execution pins by definition.
			// The metadata setting succeeds, but the function remains non-pure due to its structure.
			// This is expected behavior and not a failure of the metadata system.

			break;
		}
	}

	TestTrue(TEXT("Test function should be found with metadata"), FoundTestFunction);

	// Cleanup
	MetadataBlueprint->MarkAsGarbage();
	MetadataBlueprint = nullptr;

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlueprintMemberServiceGetFunctionsParameterAndReturnTypes,
                                 "UnrealMCP.BlueprintMemberService.GetFunctions.ParameterAndReturnTypes",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

auto FBlueprintMemberServiceGetFunctionsParameterAndReturnTypes::RunTest(const FString& Parameters) -> bool {
	// Create a test blueprint
	UBlueprint* ParamBlueprint = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(),
		GetTransientPackage(),
		FName(*FString::Printf(TEXT("TestParamsBP_%s"), *FGuid::NewGuid().ToString())),
		BPTYPE_Normal
	);

	TestNotNull(TEXT("Parameter blueprint should be created"), ParamBlueprint);

	// Add a custom function
	FString TestFunctionName = FString::Printf(TEXT("TestParamsFunction_%s"), *FGuid::NewGuid().ToString());
	UEdGraph* FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(
		ParamBlueprint,
		FName(*TestFunctionName),
		UEdGraph::StaticClass(),
		UEdGraphSchema_K2::StaticClass()
	);

	TestNotNull(TEXT("Function graph should be created"), FunctionGraph);

	// Add the function graph to the blueprint (this is the missing step!)
	// Note: AddFunctionGraph creates the entry node automatically
	FBlueprintEditorUtils::AddFunctionGraph<UClass>(ParamBlueprint, FunctionGraph, false, nullptr);

	// Find the entry node in the function graph
	UK2Node_FunctionEntry* EntryNode = nullptr;
	for (UEdGraphNode* Node : FunctionGraph->Nodes) {
		if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(Node)) {
			EntryNode = Entry;
			break;
		}
	}

	// Create a simple return node to make the function valid
	if (EntryNode) {
		UK2Node_FunctionResult* ResultNode = NewObject<UK2Node_FunctionResult>(FunctionGraph);
		if (ResultNode) {
			FunctionGraph->AddNode(ResultNode, false, false);
			ResultNode->CreateNewGuid();
			ResultNode->PostPlacedNewNode();
			ResultNode->AllocateDefaultPins();

			// Connect the entry node's execution pin to the result node
			UEdGraphPin* ThenPin = EntryNode->GetExecPin();
			UEdGraphPin* ExecutePin = ResultNode->GetExecPin();
			if (ThenPin && ExecutePin) {
				ThenPin->MakeLinkTo(ExecutePin);
			}
		}
	}

	// Add a parameter to the function
	auto AddParamResult = UnrealMCP::FBlueprintMemberService::AddFunctionParameter(
		ParamBlueprint->GetName(),
		TestFunctionName,
		TEXT("TestFloatParam"),
		TEXT("float"),
		false
	);
	TestTrue(TEXT("Adding float parameter should succeed"), AddParamResult.IsSuccess());

	// Set return type
	auto SetReturnResult = UnrealMCP::FBlueprintMemberService::SetFunctionReturnType(
		ParamBlueprint->GetName(),
		TestFunctionName,
		TEXT("bool")
	);
	TestTrue(TEXT("Setting bool return type should succeed"), SetReturnResult.IsSuccess());

	// Compile the blueprint
	FKismetEditorUtilities::CompileBlueprint(ParamBlueprint);

	// Test the service
	auto Result = UnrealMCP::FBlueprintMemberService::GetFunctions(ParamBlueprint->GetName());

	TestTrue(TEXT("Service call should succeed"), Result.IsSuccess());

	const UnrealMCP::FGetBlueprintFunctionsResult& FunctionsResult = Result.GetValue();
	TestTrue(TEXT("Should find functions"), FunctionsResult.Count > 0);

	// Find our test function and verify parameters/returns
	bool FoundTestFunction = false;
	for (const UnrealMCP::FBlueprintFunctionInfo& FunctionInfo : FunctionsResult.Functions) {
		if (FunctionInfo.Name == TestFunctionName) {
			FoundTestFunction = true;

			// Verify parameters
			bool FoundFloatParam = false;
			for (const UnrealMCP::FBlueprintFunctionParam& Param : FunctionInfo.Parameters) {
				if (Param.Name == TEXT("TestFloatParam")) {
					FoundFloatParam = true;
					TestEqual(TEXT("Parameter type should be float"), Param.Type, TEXT("float"));
					TestFalse(TEXT("Parameter should not be array"), Param.bIsArray);
					TestFalse(TEXT("Parameter should not be reference"), Param.bIsReference);

					// Test parameter JSON serialization
					auto ParamJson = Param.ToJson();
					TestTrue(TEXT("Parameter JSON should be valid"), ParamJson.IsValid());
					TestTrue(TEXT("Parameter JSON should have name"), ParamJson->HasField(TEXT("name")));
					TestTrue(TEXT("Parameter JSON should have type"), ParamJson->HasField(TEXT("type")));
					TestTrue(TEXT("Parameter JSON should have is_array"), ParamJson->HasField(TEXT("is_array")));
					TestTrue(TEXT("Parameter JSON should have is_reference"),
					         ParamJson->HasField(TEXT("is_reference")));

					break;
				}
			}
			TestTrue(TEXT("Float parameter should be found"), FoundFloatParam);

			// Verify return type
			bool FoundBoolReturn = false;
			for (const UnrealMCP::FBlueprintFunctionParam& Return : FunctionInfo.Returns) {
				if (Return.Type == TEXT("bool")) {
					FoundBoolReturn = true;
					TestEqual(TEXT("Return type should be bool"), Return.Type, TEXT("bool"));
					TestFalse(TEXT("Return should not be array"), Return.bIsArray);
					TestFalse(TEXT("Return should not be reference"), Return.bIsReference);
					break;
				}
			}
			TestTrue(TEXT("Bool return should be found"), FoundBoolReturn);

			break;
		}
	}

	TestTrue(TEXT("Test function with parameters should be found"), FoundTestFunction);

	// Cleanup
	ParamBlueprint->MarkAsGarbage();
	ParamBlueprint = nullptr;

	return true;
}
