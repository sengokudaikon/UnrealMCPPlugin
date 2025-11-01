/**
 * Unit tests for FCommonUtils::CreateEventNode function
 *
 * These tests specifically verify that the CreateEventNode function correctly
 * handles inherited events from parent classes, which was the core issue
 * that needed to be fixed in the event node creation functionality.
 */

#include "Core/CommonUtils.h"
#include "Editor.h"
#include "K2Node_Event.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/AutomationTest.h"
#include "Services/BlueprintCreationService.h"
#include "Tests/TestUtils.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCommonUtilsCreateEventNodeInheritedEventsTest,
	"UnrealMCP.CommonUtils.CreateEventNode.InheritedEvents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FCommonUtilsCreateEventNodeInheritedEventsTest::RunTest(const FString& Parameters) -> bool {
	// Test: CreateEventNode should handle inherited events from parent classes

	// Create test blueprint using MCP utilities
	const UnrealMCP::FBlueprintCreationParams CreationParams = UnrealMCPTest::FTestUtils::CreateTestBlueprintParams(
		TEXT("CommonUtilsEventTestBlueprint"));

	auto CreationResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreationParams);
	TestTrue(TEXT("Blueprint creation should succeed"), CreationResult.IsSuccess());
	UBlueprint* TestBlueprint = CreationResult.GetValue();
	TestNotNull(TEXT("Test blueprint should be created"), TestBlueprint);
	if (!TestBlueprint)
		return false;

	// Get the event graph
	UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(TestBlueprint);
	TestNotNull(TEXT("Event graph should exist"), EventGraph);
	if (!EventGraph)
		return false;

	// Map of requested event name -> expected actual function name
	TMap<FString, FString> EventNameMapping = {
		{TEXT("BeginPlay"), TEXT("ReceiveBeginPlay")},
		{TEXT("ActorBeginPlay"), TEXT("ReceiveBeginPlay")},
		{TEXT("ReceiveTick"), TEXT("ReceiveTick")},
		{TEXT("EndPlay"), TEXT("ReceiveEndPlay")}
	};

	// Test cases for inherited events that should work
	TArray<TPair<FString, FString>> TestEvents = {
		MakeTuple(TEXT("BeginPlay"), TEXT("Standard BeginPlay event from Actor class")),
		MakeTuple(TEXT("ActorBeginPlay"), TEXT("Actor-specific BeginPlay event")),
		MakeTuple(TEXT("ReceiveTick"), TEXT("Tick event from Actor class")),
		MakeTuple(TEXT("EndPlay"), TEXT("EndPlay event from Actor class"))
	};

	TArray<UK2Node_Event*> CreatedEventNodes;

	for (int32 i = 0; i < TestEvents.Num(); ++i) {
		const FString& EventName = TestEvents[i].Key;
		const FString& Description = TestEvents[i].Value;

		// Create event node using FCommonUtils::CreateEventNode
		const FVector2D NodePosition(100.0f + i * 60.0f, 100.0f + i * 90.0f);
		UK2Node_Event* EventNode = FCommonUtils::CreateEventNode(EventGraph, EventName, NodePosition);

		// Verify success
		TestNotNull(FString::Printf(TEXT("CreateEventNode should succeed for %s"), *EventName), EventNode);

		if (EventNode) {
			// Verify node properties
			TestEqual(FString::Printf(TEXT("Event node %s should be at correct position"), *EventName),
			          FVector2D(EventNode->NodePosX, EventNode->NodePosY),
			          NodePosition);

			// Check that the EventReference has the expected actual function name
			const FString& ExpectedFunctionName = EventNameMapping[EventName];
			TestTrue(FString::Printf(TEXT("Event should be %s (actual function: %s)"), *EventName, *ExpectedFunctionName),
			         EventNode->EventReference.GetMemberName() == FName(*ExpectedFunctionName));

			// Verify node has expected pins (events should have at least execution pins)
			TestTrue(FString::Printf(TEXT("Event node %s should have pins"), *EventName),
			         EventNode->Pins.Num() > 0);

			// Verify the event reference points to the correct class
			const UClass* EventSourceClass = EventNode->EventReference.GetMemberParentClass();
			TestNotNull(FString::Printf(TEXT("Event %s should have a source class"), *EventName), EventSourceClass);

			// Store for later verification
			CreatedEventNodes.Add(EventNode);

			UE_LOG(LogTemp, Display, TEXT("Successfully created event node: %s from class: %s"),
				*EventName, EventSourceClass ? *EventSourceClass->GetName() : TEXT("Unknown"));
		}
	}

	// Verify all event nodes were created successfully
	TestEqual(TEXT("Should have created all test event nodes"), CreatedEventNodes.Num(), TestEvents.Num());

	// Test that duplicate event nodes reuse existing nodes
	const UK2Node_Event* DuplicateEventNode = FCommonUtils::CreateEventNode(EventGraph, TEXT("BeginPlay"), FVector2D(500.0f, 500.0f));
	TestNotNull(TEXT("Duplicate BeginPlay event should return existing node"), DuplicateEventNode);

	// Should be the same node as the first one
	if (CreatedEventNodes.Num() > 0 && DuplicateEventNode) {
		TestTrue(TEXT("Duplicate event should return existing node"),
		         CreatedEventNodes[0] == DuplicateEventNode);
		TestEqual(TEXT("Duplicate event should have updated position"),
		          FVector2D(DuplicateEventNode->NodePosX, DuplicateEventNode->NodePosY),
		          FVector2D(500.0f, 500.0f));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCommonUtilsCreateEventNodeInvalidEventsTest,
	"UnrealMCP.CommonUtils.CreateEventNode.InvalidEvents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FCommonUtilsCreateEventNodeInvalidEventsTest::RunTest(const FString& Parameters) -> bool {
	// Test: CreateEventNode should return nullptr for non-existent events

	// Create test blueprint using MCP utilities
	const UnrealMCP::FBlueprintCreationParams CreationParams = UnrealMCPTest::FTestUtils::CreateTestBlueprintParams(
		TEXT("CommonUtilsInvalidEventTestBlueprint"));

	auto CreationResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreationParams);
	TestTrue(TEXT("Blueprint creation should succeed"), CreationResult.IsSuccess());
	UBlueprint* TestBlueprint = CreationResult.GetValue();
	TestNotNull(TEXT("Test blueprint should be created"), TestBlueprint);
	if (!TestBlueprint)
		return false;

	// Get the event graph
	UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(TestBlueprint);
	TestNotNull(TEXT("Event graph should exist"), EventGraph);
	if (!EventGraph)
		return false;

	// Test invalid event names that should fail
	TArray<FString> InvalidEvents = {
		TEXT("NonExistentEvent"),
		TEXT("InvalidEvent123"),
		TEXT("ThisEventDoesNotExist"),
		TEXT("FakeBeginPlay")
	};

	for (const FString& EventName : InvalidEvents) {
		// Try to create invalid event node
		const FVector2D NodePosition(100.0f, 100.0f);
		const UK2Node_Event* EventNode = FCommonUtils::CreateEventNode(EventGraph, EventName, NodePosition);

		// Verify failure (should return nullptr)
		TestNull(FString::Printf(TEXT("CreateEventNode should return nullptr for %s"), *EventName), EventNode);
	}

	// Test with null graph (should return nullptr)
	const UK2Node_Event* NullGraphNode = FCommonUtils::CreateEventNode(nullptr, TEXT("BeginPlay"), FVector2D::ZeroVector);
	TestNull(TEXT("CreateEventNode should return nullptr for null graph"), NullGraphNode);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCommonUtilsCreateEventNodeClassHierarchyTest,
	"UnrealMCP.CommonUtils.CreateEventNode.ClassHierarchy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FCommonUtilsCreateEventNodeClassHierarchyTest::RunTest(const FString& Parameters) -> bool {
	// Test: CreateEventNode should find events from different levels in class hierarchy

	// Create test blueprint using MCP utilities
	const UnrealMCP::FBlueprintCreationParams CreationParams = UnrealMCPTest::FTestUtils::CreateTestBlueprintParams(
		TEXT("ClassHierarchyTestBlueprint"));

	auto CreationResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CreationParams);
	TestTrue(TEXT("Blueprint creation should succeed"), CreationResult.IsSuccess());
	UBlueprint* TestBlueprint = CreationResult.GetValue();
	TestNotNull(TEXT("Test blueprint should be created"), TestBlueprint);
	if (!TestBlueprint)
		return false;

	// Get the event graph
	UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(TestBlueprint);
	TestNotNull(TEXT("Event graph should exist"), EventGraph);
	if (!EventGraph)
		return false;

	// Test events from different levels of the class hierarchy
	TArray<TPair<FString, UClass*>> TestEventsWithExpectedClasses;

	// Since we create Actor-based blueprints, these should come from Actor class
	TestEventsWithExpectedClasses.Add(MakeTuple(TEXT("BeginPlay"), AActor::StaticClass()));
	TestEventsWithExpectedClasses.Add(MakeTuple(TEXT("ReceiveTick"), AActor::StaticClass()));
	TestEventsWithExpectedClasses.Add(MakeTuple(TEXT("EndPlay"), AActor::StaticClass()));

	for (const auto& EventTest : TestEventsWithExpectedClasses) {
		const FString& EventName = EventTest.Key;
		const UClass* ExpectedClass = EventTest.Value;

		// Create event node
		const FVector2D NodePosition(100.0f, 100.0f);
		const UK2Node_Event* EventNode = FCommonUtils::CreateEventNode(EventGraph, EventName, NodePosition);

		TestNotNull(FString::Printf(TEXT("Event node %s should be created"), *EventName), EventNode);

		if (EventNode) {
			// Verify the event reference points to a class in the correct hierarchy
			const UClass* EventSourceClass = EventNode->EventReference.GetMemberParentClass();
			TestNotNull(FString::Printf(TEXT("Event %s should have a source class"), *EventName), EventSourceClass);

			if (EventSourceClass) {
				// Verify the source class is either the expected class or a parent of it
				const bool bIsCorrectClass = (EventSourceClass == ExpectedClass) ||
				                       EventSourceClass->IsChildOf(ExpectedClass) ||
				                       ExpectedClass->IsChildOf(EventSourceClass);

				TestTrue(FString::Printf(TEXT("Event %s source class should be in correct hierarchy: %s"),
				         *EventName, *EventSourceClass->GetName()), bIsCorrectClass);
			}
		}
	}

	return true;
}