/**
 * Functional tests for BlueprintIntrospectionService
 *
 * These tests verify the actual behavior of blueprint introspection operations:
 * - Creating real blueprints with various component structures
 * - Listing blueprints and verifying path resolution
 * - Extracting detailed blueprint information and metadata
 * - Getting component properties with type-specific data
 * - Building component hierarchies with parent-child relationships
 * - Listing blueprint variables with type and property metadata
 * - Component modification operations (remove, rename)
 * - Edge cases and boundary conditions
 *
 * Tests focus on functional outcomes and behavioral verification
 * rather than simple existence checks or error handling.
 */

#include "Editor.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "Services/BlueprintCreationService.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Services/BlueprintService.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceCreateTestBlueprintsTest,
	"UnrealMCP.BlueprintIntrospection.CreateTestBlueprints",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceCreateTestBlueprintsTest::RunTest(const FString& Parameters) -> bool {
	// Test: Create multiple test blueprints with different structures for introspection testing

	// Create Actor-based blueprint with basic components
	UnrealMCP::FBlueprintCreationParams ActorParams;
	ActorParams.Name = TEXT("ActorTestBlueprint");
	ActorParams.PackagePath = TEXT("/Game/Tests/Introspection/");
	ActorParams.ParentClass = TEXT("Actor");

	const auto ActorBlueprintResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(ActorParams);
	TestTrue(TEXT("Actor blueprint should be created"), ActorBlueprintResult.IsSuccess());

	// Create Character-based blueprint with movement component
	UnrealMCP::FBlueprintCreationParams CharacterParams;
	CharacterParams.Name = TEXT("CharacterTestBlueprint");
	CharacterParams.PackagePath = TEXT("/Game/Tests/Introspection/");
	CharacterParams.ParentClass = TEXT("Character");

	const auto CharacterBlueprintResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(CharacterParams);
	TestTrue(TEXT("Character blueprint should be created"), CharacterBlueprintResult.IsSuccess());

	// Add components to Actor blueprint for testing
	UnrealMCP::FComponentParams RootComponentParams;
	RootComponentParams.BlueprintName = TEXT("ActorTestBlueprint");
	RootComponentParams.ComponentName = TEXT("RootSceneComponent");
	RootComponentParams.ComponentType = TEXT("SceneComponent");

	const auto RootResult = UnrealMCP::FBlueprintService::AddComponent(RootComponentParams);
	TestTrue(TEXT("Root component should be added"), RootResult.IsSuccess());

	UnrealMCP::FComponentParams MeshComponentParams;
	MeshComponentParams.BlueprintName = TEXT("ActorTestBlueprint");
	MeshComponentParams.ComponentName = TEXT("TestMeshComponent");
	MeshComponentParams.ComponentType = TEXT("StaticMeshComponent");

	const auto MeshResult = UnrealMCP::FBlueprintService::AddComponent(MeshComponentParams);
	TestTrue(TEXT("Mesh component should be added"), MeshResult.IsSuccess());

	UnrealMCP::FComponentParams LightComponentParams;
	LightComponentParams.BlueprintName = TEXT("ActorTestBlueprint");
	LightComponentParams.ComponentName = TEXT("TestLightComponent");
	LightComponentParams.ComponentType = TEXT("PointLightComponent");

	const auto LightResult = UnrealMCP::FBlueprintService::AddComponent(LightComponentParams);
	TestTrue(TEXT("Light component should be added"), LightResult.IsSuccess());

	// Verify we can find the created blueprints
	TestTrue(TEXT("Actor blueprint should exist via service"),
	         UnrealMCP::FBlueprintIntrospectionService::BlueprintExists(TEXT("ActorTestBlueprint")));
	TestTrue(TEXT("Character blueprint should exist via service"),
	         UnrealMCP::FBlueprintIntrospectionService::BlueprintExists(TEXT("CharacterTestBlueprint")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceListBlueprintsTest,
	"UnrealMCP.BlueprintIntrospection.ListBlueprints",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceListBlueprintsTest::RunTest(const FString& Parameters) -> bool {
	// Test: List blueprints in directories and verify results

	// Create test blueprints first
	UnrealMCP::FBlueprintCreationParams Params1;
	Params1.Name = TEXT("ListTest1");
	Params1.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params1.ParentClass = TEXT("Actor");

	const auto Result1 = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params1);
	TestTrue(TEXT("First test blueprint should be created"), Result1.IsSuccess());

	UnrealMCP::FBlueprintCreationParams Params2;
	Params2.Name = TEXT("ListTest2");
	Params2.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params2.ParentClass = TEXT("Actor");

	const auto Result2 = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params2);
	TestTrue(TEXT("Second test blueprint should be created"), Result2.IsSuccess());

	// Test listing from test directory (non-recursive)
	TArray<FString> Blueprints;
	UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintIntrospectionService::ListBlueprints(
		TEXT("/Game/Tests/Introspection"),
		false,
		Blueprints);

	TestTrue(TEXT("ListBlueprints should succeed"), Result.IsSuccess());
	TestTrue(TEXT("Should find at least 2 blueprints"), Blueprints.Num() >= 2);

	// Verify our test blueprints are in the list
	bool bFoundTest1 = false, bFoundTest2 = false;
	for (const FString& Blueprint : Blueprints) {
		if (Blueprint.Contains(TEXT("ListTest1")))
			bFoundTest1 = true;
		if (Blueprint.Contains(TEXT("ListTest2")))
			bFoundTest2 = true;
	}
	TestTrue(TEXT("Should find ListTest1 blueprint"), bFoundTest1);
	TestTrue(TEXT("Should find ListTest2 blueprint"), bFoundTest2);

	// Test recursive listing from /Game
	Blueprints.Empty();
	Result = UnrealMCP::FBlueprintIntrospectionService::ListBlueprints(
		TEXT("/Game"),
		true,
		Blueprints);

	TestTrue(TEXT("Recursive listing should succeed"), Result.IsSuccess());
	TestTrue(TEXT("Should find multiple blueprints in /Game"), Blueprints.Num() > 0);

	// Test non-existent directory
	Blueprints.Empty();
	Result = UnrealMCP::FBlueprintIntrospectionService::ListBlueprints(
		TEXT("/Game/NonExistentDirectory_XYZ123"),
		true,
		Blueprints);

	TestTrue(TEXT("Should handle non-existent directory gracefully"), Result.IsSuccess());
	TestEqual(TEXT("Should return empty array for non-existent directory"), Blueprints.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetBlueprintInfoTest,
	"UnrealMCP.BlueprintIntrospection.GetBlueprintInfo",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceGetBlueprintInfoTest::RunTest(const FString& Parameters) -> bool {
	// Test: Get detailed blueprint information and verify data accuracy

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("InfoTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Character");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Add a component to test metadata
	UnrealMCP::FComponentParams ComponentParams;
	ComponentParams.BlueprintName = TEXT("InfoTestBlueprint");
	ComponentParams.ComponentName = TEXT("TestSceneComponent");
	ComponentParams.ComponentType = TEXT("SceneComponent");

	const auto ComponentResult = UnrealMCP::FBlueprintService::AddComponent(ComponentParams);
	TestTrue(TEXT("Component should be added"), ComponentResult.IsSuccess());

	// Get blueprint info
	TMap<FString, FString> OutInfo;
	UnrealMCP::FVoidResult InfoResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintInfo(
		TEXT("InfoTestBlueprint"),
		OutInfo);

	TestTrue(TEXT("GetBlueprintInfo should succeed"), InfoResult.IsSuccess());
	TestTrue(TEXT("Should have multiple info fields"), OutInfo.Num() > 0);

	// Verify required fields exist
	TestTrue(TEXT("Should have name field"), OutInfo.Contains(TEXT("name")));
	TestTrue(TEXT("Should have path field"), OutInfo.Contains(TEXT("path")));
	TestTrue(TEXT("Should have parent_class field"), OutInfo.Contains(TEXT("parent_class")));
	TestTrue(TEXT("Should have blueprint_type field"), OutInfo.Contains(TEXT("blueprint_type")));
	TestTrue(TEXT("Should have num_components field"), OutInfo.Contains(TEXT("num_components")));
	TestTrue(TEXT("Should have num_variables field"), OutInfo.Contains(TEXT("num_variables")));

	// Verify field values
	TestEqual(TEXT("Name should match"), OutInfo[TEXT("name")], TEXT("InfoTestBlueprint"));
	TestTrue(TEXT("Path should contain blueprint name"), OutInfo[TEXT("path")].Contains(TEXT("InfoTestBlueprint")));
	TestEqual(TEXT("Parent class should be Character"), OutInfo[TEXT("parent_class")], TEXT("Character"));
	TestEqual(TEXT("Blueprint type should be Normal"), OutInfo[TEXT("blueprint_type")], TEXT("Normal"));
	TestTrue(TEXT("Should have at least 1 component"), FCString::Atoi(*OutInfo[TEXT("num_components")]) >= 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetBlueprintVariablesTest,
	"UnrealMCP.BlueprintIntrospection.GetBlueprintVariables",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceGetBlueprintVariablesTest::RunTest(const FString& Parameters) -> bool {
	// Test: Get blueprint variables with detailed type and property information

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("VariablesTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Actor");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Get variables (should be empty initially)
	const auto VariablesResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintVariables(
		TEXT("VariablesTestBlueprint"));

	TestTrue(TEXT("GetBlueprintVariables should succeed"), VariablesResult.IsSuccess());
	const auto& VariablesData = VariablesResult.GetValue();
	TestEqual(TEXT("Should have 0 variables initially"), VariablesData.Variables.Num(), 0);
	TestEqual(TEXT("Count should be 0"), VariablesData.Count, 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetBlueprintComponentsTest,
	"UnrealMCP.BlueprintIntrospection.GetBlueprintComponents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceGetBlueprintComponentsTest::RunTest(const FString& Parameters) -> bool {
	// Test: Get blueprint components with transform and type information

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("ComponentsTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Actor");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Add different types of components
	UnrealMCP::FComponentParams RootParams;
	RootParams.BlueprintName = TEXT("ComponentsTestBlueprint");
	RootParams.ComponentName = TEXT("TestRoot");
	RootParams.ComponentType = TEXT("SceneComponent");

	const auto RootResult = UnrealMCP::FBlueprintService::AddComponent(RootParams);
	TestTrue(TEXT("Root component should be added"), RootResult.IsSuccess());

	UnrealMCP::FComponentParams MeshParams;
	MeshParams.BlueprintName = TEXT("ComponentsTestBlueprint");
	MeshParams.ComponentName = TEXT("TestMesh");
	MeshParams.ComponentType = TEXT("StaticMeshComponent");
	MeshParams.Location = FVector(100.0f, 200.0f, 300.0f);
	MeshParams.Rotation = FRotator(45.0f, 90.0f, 0.0f);
	MeshParams.Scale = FVector(2.0f, 1.5f, 0.5f);

	const auto MeshResult = UnrealMCP::FBlueprintService::AddComponent(MeshParams);
	TestTrue(TEXT("Mesh component should be added"), MeshResult.IsSuccess());

	UnrealMCP::FComponentParams BoxParams;
	BoxParams.BlueprintName = TEXT("ComponentsTestBlueprint");
	BoxParams.ComponentName = TEXT("TestBox");
	BoxParams.ComponentType = TEXT("BoxComponent");

	const auto BoxResult = UnrealMCP::FBlueprintService::AddComponent(BoxParams);
	TestTrue(TEXT("Box component should be added"), BoxResult.IsSuccess());

	// Get components
	TArray<TMap<FString, FString>> OutComponents;
	UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintComponents(
		TEXT("ComponentsTestBlueprint"),
		OutComponents);

	TestTrue(TEXT("GetBlueprintComponents should succeed"), Result.IsSuccess());
	TestTrue(TEXT("Should have multiple components"), OutComponents.Num() >= 3);

	// Verify component data
	bool bFoundRoot = false, bFoundMesh = false, bFoundBox = false;
	for (const TMap<FString, FString>& Component : OutComponents) {
		const FString* Name = Component.Find(TEXT("name"));
		const FString* Type = Component.Find(TEXT("type"));

		if (Name && Type) {
			if (*Name == TEXT("TestRoot")) {
				bFoundRoot = true;
				TestEqual(TEXT("Root should be SceneComponent"), *Type, TEXT("SceneComponent"));
			}
			else if (*Name == TEXT("TestMesh")) {
				bFoundMesh = true;
				TestEqual(TEXT("Mesh should be StaticMeshComponent"), *Type, TEXT("StaticMeshComponent"));
			}
			else if (*Name == TEXT("TestBox")) {
				bFoundBox = true;
				TestEqual(TEXT("Box should be BoxComponent"), *Type, TEXT("BoxComponent"));
			}
		}
	}

	TestTrue(TEXT("Should find root component"), bFoundRoot);
	TestTrue(TEXT("Should find mesh component"), bFoundMesh);
	TestTrue(TEXT("Should find box component"), bFoundBox);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetComponentPropertiesTest,
	"UnrealMCP.BlueprintIntrospection.GetComponentProperties",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceGetComponentPropertiesTest::RunTest(const FString& Parameters) -> bool {
	// Test: Get detailed component properties with type-specific data

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("PropertiesTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Actor");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Add components with different types for property testing
	UnrealMCP::FComponentParams MeshParams;
	MeshParams.BlueprintName = TEXT("PropertiesTestBlueprint");
	MeshParams.ComponentName = TEXT("TestMesh");
	MeshParams.ComponentType = TEXT("StaticMeshComponent");
	MeshParams.Location = FVector(150.0f, 250.0f, 350.0f);

	const auto MeshResult = UnrealMCP::FBlueprintService::AddComponent(MeshParams);
	TestTrue(TEXT("Mesh component should be added"), MeshResult.IsSuccess());

	UnrealMCP::FComponentParams LightParams;
	LightParams.BlueprintName = TEXT("PropertiesTestBlueprint");
	LightParams.ComponentName = TEXT("TestLight");
	LightParams.ComponentType = TEXT("PointLightComponent");

	const auto LightResult = UnrealMCP::FBlueprintService::AddComponent(LightParams);
	TestTrue(TEXT("Light component should be added"), LightResult.IsSuccess());

	UnrealMCP::FComponentParams PhysicsParams;
	PhysicsParams.BlueprintName = TEXT("PropertiesTestBlueprint");
	PhysicsParams.ComponentName = TEXT("TestPhysics");
	PhysicsParams.ComponentType = TEXT("BoxComponent");

	const auto PhysicsResult = UnrealMCP::FBlueprintService::AddComponent(PhysicsParams);
	TestTrue(TEXT("Physics component should be added"), PhysicsResult.IsSuccess());

	// Test StaticMeshComponent properties
	UnrealMCP::FComponentPropertiesParams GetMeshParams;
	GetMeshParams.BlueprintName = TEXT("PropertiesTestBlueprint");
	GetMeshParams.ComponentName = TEXT("TestMesh");

	const auto GetMeshResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(GetMeshParams);
	TestTrue(TEXT("GetComponentProperties should succeed for mesh"), GetMeshResult.IsSuccess());

	const auto& MeshProperties = GetMeshResult.GetValue().Properties;
	TestNotNull(TEXT("Mesh properties should not be null"), MeshProperties.Get());
	if (MeshProperties) {
		TestEqual(TEXT("Mesh name should match"), MeshProperties->GetStringField(TEXT("name")), TEXT("TestMesh"));
		TestEqual(TEXT("Mesh type should be StaticMeshComponent"),
		          MeshProperties->GetStringField(TEXT("type")),
		          TEXT("StaticMeshComponent"));
		TestTrue(TEXT("Mesh should have transform"), MeshProperties->HasField(TEXT("transform")));
	}

	// Test PointLightComponent properties
	UnrealMCP::FComponentPropertiesParams GetLightParams;
	GetLightParams.BlueprintName = TEXT("PropertiesTestBlueprint");
	GetLightParams.ComponentName = TEXT("TestLight");

	const auto GetLightResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(GetLightParams);
	TestTrue(TEXT("GetComponentProperties should succeed for light"), GetLightResult.IsSuccess());

	const auto& LightProperties = GetLightResult.GetValue().Properties;
	TestNotNull(TEXT("Light properties should not be null"), LightProperties.Get());
	if (LightProperties) {
		TestEqual(TEXT("Light name should match"), LightProperties->GetStringField(TEXT("name")), TEXT("TestLight"));
		TestEqual(TEXT("Light type should be PointLightComponent"),
		          LightProperties->GetStringField(TEXT("type")),
		          TEXT("PointLightComponent"));
		TestTrue(TEXT("Light should have light properties"), LightProperties->HasField(TEXT("light")));
	}

	// Test BoxComponent physics properties
	UnrealMCP::FComponentPropertiesParams GetPhysicsParams;
	GetPhysicsParams.BlueprintName = TEXT("PropertiesTestBlueprint");
	GetPhysicsParams.ComponentName = TEXT("TestPhysics");

	const auto GetPhysicsResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(GetPhysicsParams);
	TestTrue(TEXT("GetComponentProperties should succeed for physics"), GetPhysicsResult.IsSuccess());

	const auto& PhysicsProperties = GetPhysicsResult.GetValue().Properties;
	TestNotNull(TEXT("Physics properties should not be null"), PhysicsProperties.Get());
	if (PhysicsProperties) {
		TestTrue(TEXT("Physics should have physics properties"), PhysicsProperties->HasField(TEXT("physics")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetComponentHierarchyTest,
	"UnrealMCP.BlueprintIntrospection.GetComponentHierarchy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceGetComponentHierarchyTest::RunTest(const FString& Parameters) -> bool {
	// Test: Build component hierarchy and verify parent-child relationships

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("HierarchyTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Actor");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Create hierarchical component structure
	UnrealMCP::FComponentParams RootParams;
	RootParams.BlueprintName = TEXT("HierarchyTestBlueprint");
	RootParams.ComponentName = TEXT("SceneRoot");
	RootParams.ComponentType = TEXT("SceneComponent");

	const auto RootResult = UnrealMCP::FBlueprintService::AddComponent(RootParams);
	TestTrue(TEXT("Root component should be added"), RootResult.IsSuccess());

	UnrealMCP::FComponentParams MeshParams;
	MeshParams.BlueprintName = TEXT("HierarchyTestBlueprint");
	MeshParams.ComponentName = TEXT("MeshComponent");
	MeshParams.ComponentType = TEXT("StaticMeshComponent");

	const auto MeshResult = UnrealMCP::FBlueprintService::AddComponent(MeshParams);
	TestTrue(TEXT("Mesh component should be added"), MeshResult.IsSuccess());

	UnrealMCP::FComponentParams LightParams;
	LightParams.BlueprintName = TEXT("HierarchyTestBlueprint");
	LightParams.ComponentName = TEXT("LightComponent");
	LightParams.ComponentType = TEXT("PointLightComponent");

	const auto LightResult = UnrealMCP::FBlueprintService::AddComponent(LightParams);
	TestTrue(TEXT("Light component should be added"), LightResult.IsSuccess());

	UnrealMCP::FComponentParams AttachmentParams;
	AttachmentParams.BlueprintName = TEXT("HierarchyTestBlueprint");
	AttachmentParams.ComponentName = TEXT("AttachmentComponent");
	AttachmentParams.ComponentType = TEXT("SceneComponent");

	const auto AttachmentResult = UnrealMCP::FBlueprintService::AddComponent(AttachmentParams);
	TestTrue(TEXT("Attachment component should be added"), AttachmentResult.IsSuccess());

	// Get component hierarchy
	UnrealMCP::FComponentHierarchyParams HierarchyParams;
	HierarchyParams.BlueprintName = TEXT("HierarchyTestBlueprint");

	const auto HierarchyResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentHierarchy(HierarchyParams);
	TestTrue(TEXT("GetComponentHierarchy should succeed"), HierarchyResult.IsSuccess());

	const auto& HierarchyData = HierarchyResult.GetValue();
	TestTrue(TEXT("Should have root components"), HierarchyData.RootCount > 0);
	TestTrue(TEXT("Should have total components"), HierarchyData.TotalComponents >= 4);
	TestTrue(TEXT("Should have hierarchy data"), HierarchyData.Hierarchy.Num() > 0);

	// Verify hierarchy structure
	bool bFoundRoot = false, bFoundMesh = false, bFoundLight = false, bFoundAttachment = false;
	for (const auto& NodeValue : HierarchyData.Hierarchy) {
		const auto NodeObj = NodeValue->AsObject();
		if (NodeObj) {
			const FString NodeName = NodeObj->GetStringField(TEXT("name"));

			if (NodeName == TEXT("SceneRoot")) {
				bFoundRoot = true;
				TestTrue(TEXT("Root should be marked as root"), NodeObj->GetBoolField(TEXT("is_root")));
				TestTrue(TEXT("Root should be scene component"), NodeObj->GetBoolField(TEXT("is_scene_component")));
			}
			else if (NodeName == TEXT("MeshComponent")) {
				bFoundMesh = true;
				TestTrue(TEXT("Mesh should be scene component"), NodeObj->GetBoolField(TEXT("is_scene_component")));
			}
			else if (NodeName == TEXT("LightComponent")) {
				bFoundLight = true;
				TestEqual(TEXT("Light type should be PointLightComponent"),
				          NodeObj->GetStringField(TEXT("type")),
				          TEXT("PointLightComponent"));
			}
			else if (NodeName == TEXT("AttachmentComponent")) {
				bFoundAttachment = true;
			}
		}
	}

	TestTrue(TEXT("Should find root component"), bFoundRoot);
	TestTrue(TEXT("Should find mesh component"), bFoundMesh);
	TestTrue(TEXT("Should find light component"), bFoundLight);
	TestTrue(TEXT("Should find attachment component"), bFoundAttachment);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRemoveComponentTest,
	"UnrealMCP.BlueprintIntrospection.RemoveComponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceRemoveComponentTest::RunTest(const FString& Parameters) -> bool {
	// Test: Remove component from blueprint and verify removal

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("RemoveTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Actor");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Add components to blueprint
	UnrealMCP::FComponentParams KeepParams;
	KeepParams.BlueprintName = TEXT("RemoveTestBlueprint");
	KeepParams.ComponentName = TEXT("KeepComponent");
	KeepParams.ComponentType = TEXT("SceneComponent");

	const auto KeepResult = UnrealMCP::FBlueprintService::AddComponent(KeepParams);
	TestTrue(TEXT("Keep component should be added"), KeepResult.IsSuccess());

	UnrealMCP::FComponentParams RemoveParams;
	RemoveParams.BlueprintName = TEXT("RemoveTestBlueprint");
	RemoveParams.ComponentName = TEXT("RemoveComponent");
	RemoveParams.ComponentType = TEXT("SceneComponent");

	const auto RemoveAddResult = UnrealMCP::FBlueprintService::AddComponent(RemoveParams);
	TestTrue(TEXT("Remove component should be added"), RemoveAddResult.IsSuccess());

	UnrealMCP::FComponentParams AnotherParams;
	AnotherParams.BlueprintName = TEXT("RemoveTestBlueprint");
	AnotherParams.ComponentName = TEXT("AnotherComponent");
	AnotherParams.ComponentType = TEXT("SceneComponent");

	const auto AnotherResult = UnrealMCP::FBlueprintService::AddComponent(AnotherParams);
	TestTrue(TEXT("Another component should be added"), AnotherResult.IsSuccess());

	// Verify components exist initially
	TArray<TMap<FString, FString>> InitialComponents;
	UnrealMCP::FVoidResult InitialResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintComponents(
		TEXT("RemoveTestBlueprint"),
		InitialComponents);
	TestTrue(TEXT("Should get initial components"), InitialResult.IsSuccess());
	TestTrue(TEXT("Should have 3 components initially"), InitialComponents.Num() >= 3);

	// Remove one component
	UnrealMCP::FRemoveComponentParams RemoveComponentParams;
	RemoveComponentParams.BlueprintName = TEXT("RemoveTestBlueprint");
	RemoveComponentParams.ComponentName = TEXT("RemoveComponent");

	const auto RemoveResult = UnrealMCP::FBlueprintIntrospectionService::RemoveComponent(RemoveComponentParams);
	TestTrue(TEXT("removeComponent should succeed"), RemoveResult.IsSuccess());

	const auto& RemoveData = RemoveResult.GetValue();
	TestEqual(TEXT("Blueprint name should match"), RemoveData.BlueprintName, TEXT("RemoveTestBlueprint"));
	TestEqual(TEXT("Component name should match"), RemoveData.ComponentName, TEXT("RemoveComponent"));
	TestTrue(TEXT("Message should confirm removal"),
	         RemoveData.Message.Contains(TEXT("removed")) && RemoveData.Message.Contains(TEXT("RemoveComponent")));

	// Verify component was removed
	TArray<TMap<FString, FString>> FinalComponents;
	UnrealMCP::FVoidResult FinalResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintComponents(
		TEXT("RemoveTestBlueprint"),
		FinalComponents);
	TestTrue(TEXT("Should get final components"), FinalResult.IsSuccess());

	// Verify removed component is no longer present
	bool bFoundRemoved = false, bFoundKept = false, bFoundAnother = false;
	for (const TMap<FString, FString>& Component : FinalComponents) {
		const FString* Name = Component.Find(TEXT("name"));
		if (Name) {
			if (*Name == TEXT("RemoveComponent"))
				bFoundRemoved = true;
			if (*Name == TEXT("KeepComponent"))
				bFoundKept = true;
			if (*Name == TEXT("AnotherComponent"))
				bFoundAnother = true;
		}
	}

	TestFalse(TEXT("Removed component should not be found"), bFoundRemoved);
	TestTrue(TEXT("Kept component should still exist"), bFoundKept);
	TestTrue(TEXT("Another component should still exist"), bFoundAnother);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceRenameComponentTest,
	"UnrealMCP.BlueprintIntrospection.RenameComponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceRenameComponentTest::RunTest(const FString& Parameters) -> bool {
	// Test: Rename component in blueprint and verify name change

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("RenameTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Actor");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Add components to blueprint
	UnrealMCP::FComponentParams OriginalParams;
	OriginalParams.BlueprintName = TEXT("RenameTestBlueprint");
	OriginalParams.ComponentName = TEXT("OriginalName");
	OriginalParams.ComponentType = TEXT("SceneComponent");

	const auto OriginalResult = UnrealMCP::FBlueprintService::AddComponent(OriginalParams);
	TestTrue(TEXT("Original component should be added"), OriginalResult.IsSuccess());

	UnrealMCP::FComponentParams OtherParams;
	OtherParams.BlueprintName = TEXT("RenameTestBlueprint");
	OtherParams.ComponentName = TEXT("OtherComponent");
	OtherParams.ComponentType = TEXT("SceneComponent");

	const auto OtherResult = UnrealMCP::FBlueprintService::AddComponent(OtherParams);
	TestTrue(TEXT("Other component should be added"), OtherResult.IsSuccess());

	// Verify original component exists
	TArray<TMap<FString, FString>> InitialComponents;
	UnrealMCP::FVoidResult InitialResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintComponents(
		TEXT("RenameTestBlueprint"),
		InitialComponents);
	TestTrue(TEXT("Should get initial components"), InitialResult.IsSuccess());

	bool bFoundOriginal = false;
	for (const TMap<FString, FString>& Component : InitialComponents) {
		const FString* Name = Component.Find(TEXT("name"));
		if (Name && *Name == TEXT("OriginalName")) {
			bFoundOriginal = true;
			break;
		}
	}
	TestTrue(TEXT("Original component should exist initially"), bFoundOriginal);

	// Rename the component
	UnrealMCP::FRenameComponentParams RenameParams;
	RenameParams.BlueprintName = TEXT("RenameTestBlueprint");
	RenameParams.OldName = TEXT("OriginalName");
	RenameParams.NewName = TEXT("NewName");

	const auto RenameResult = UnrealMCP::FBlueprintIntrospectionService::RenameComponent(RenameParams);
	TestTrue(TEXT("renameComponent should succeed"), RenameResult.IsSuccess());

	const auto& RenameData = RenameResult.GetValue();
	TestEqual(TEXT("Blueprint name should match"), RenameData.BlueprintName, TEXT("RenameTestBlueprint"));
	TestEqual(TEXT("Old name should match"), RenameData.OldName, TEXT("OriginalName"));
	TestEqual(TEXT("New name should match"), RenameData.NewName, TEXT("NewName"));
	TestTrue(TEXT("Message should confirm rename"),
	         RenameData.Message.Contains(TEXT("renamed")) &&
	         RenameData.Message.Contains(TEXT("OriginalName")) &&
	         RenameData.Message.Contains(TEXT("NewName")));

	// Verify component was renamed
	TArray<TMap<FString, FString>> FinalComponents;
	UnrealMCP::FVoidResult FinalResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintComponents(
		TEXT("RenameTestBlueprint"),
		FinalComponents);
	TestTrue(TEXT("Should get final components"), FinalResult.IsSuccess());

	bFoundOriginal = false; bool bFoundNew = false, bFoundOther = false;
	for (const TMap<FString, FString>& Component : FinalComponents) {
		const FString* Name = Component.Find(TEXT("name"));
		if (Name) {
			if (*Name == TEXT("OriginalName"))
				bFoundOriginal = true;
			if (*Name == TEXT("NewName"))
				bFoundNew = true;
			if (*Name == TEXT("OtherComponent"))
				bFoundOther = true;
		}
	}

	TestFalse(TEXT("Original name should not be found"), bFoundOriginal);
	TestTrue(TEXT("New name should be found"), bFoundNew);
	TestTrue(TEXT("Other component should still exist"), bFoundOther);

	// Test duplicate name error (try to rename to existing name)
	UnrealMCP::FRenameComponentParams DuplicateParams;
	DuplicateParams.BlueprintName = TEXT("RenameTestBlueprint");
	DuplicateParams.OldName = TEXT("NewName");
	DuplicateParams.NewName = TEXT("OtherComponent"); // Already exists

	const auto DuplicateResult = UnrealMCP::FBlueprintIntrospectionService::RenameComponent(DuplicateParams);
	TestTrue(TEXT("Should fail for duplicate name"), DuplicateResult.IsFailure());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceGetBlueprintPathTest,
	"UnrealMCP.BlueprintIntrospection.GetBlueprintPath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceGetBlueprintPathTest::RunTest(const FString& Parameters) -> bool {
	// Test: Get blueprint path and verify path resolution

	// Create test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("PathTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Actor");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Test blueprint should be created"), CreateResult.IsSuccess());

	// Test getting path by short name
	const FString ShortNamePath = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintPath(
		TEXT("PathTestBlueprint"));
	TestTrue(TEXT("Should get path for short name"), !ShortNamePath.IsEmpty());
	TestTrue(TEXT("Path should contain blueprint name"), ShortNamePath.Contains(TEXT("PathTestBlueprint")));
	TestTrue(TEXT("Path should contain /Game/"), ShortNamePath.Contains(TEXT("/Game/")));

	// Test getting path by full path
	const FString FullPath = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintPath(
		TEXT("/Game/Tests/Introspection/PathTestBlueprint"));
	TestTrue(TEXT("Should get path for full path"), !FullPath.IsEmpty());
	TestEqual(TEXT("Full path should match short name path"), FullPath, ShortNamePath);

	// Test non-existent blueprint
	const FString NonExistentPath = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintPath(
		TEXT("NonExistentBlueprint_XYZ123"));
	TestTrue(TEXT("Should return empty for non-existent blueprint"), NonExistentPath.IsEmpty());

	// Test empty name
	const FString EmptyNamePath = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintPath(TEXT(""));
	TestTrue(TEXT("Should return empty for empty name"), EmptyNamePath.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceErrorHandlingTest,
	"UnrealMCP.BlueprintIntrospection.ErrorHandling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceErrorHandlingTest::RunTest(const FString& Parameters) -> bool {
	// Test: Proper error handling for invalid operations

	// Test non-existent blueprint operations
	TMap<FString, FString> Info;
	UnrealMCP::FVoidResult InfoResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintInfo(
		TEXT("NonExistentBlueprint_XYZ123"),
		Info);
	TestTrue(TEXT("GetBlueprintInfo should fail for non-existent blueprint"), InfoResult.IsFailure());

	const auto VariablesResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintVariables(
		TEXT("NonExistentBlueprint_XYZ123"));
	TestTrue(TEXT("GetBlueprintVariables should fail for non-existent blueprint"), VariablesResult.IsFailure());

	TArray<TMap<FString, FString>> Components;
	UnrealMCP::FVoidResult ComponentsResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintComponents(
		TEXT("NonExistentBlueprint_XYZ123"),
		Components);
	TestTrue(TEXT("GetBlueprintComponents should fail for non-existent blueprint"), ComponentsResult.IsFailure());

	// Test component operations on non-existent blueprint
	UnrealMCP::FComponentPropertiesParams PropsParams;
	PropsParams.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");
	PropsParams.ComponentName = TEXT("SomeComponent");

	const auto PropsResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(PropsParams);
	TestTrue(TEXT("GetComponentProperties should fail for non-existent blueprint"), PropsResult.IsFailure());

	UnrealMCP::FComponentHierarchyParams HierarchyParams;
	HierarchyParams.BlueprintName = TEXT("NonExistentBlueprint_XYZ123");

	const auto HierarchyResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentHierarchy(HierarchyParams);
	TestTrue(TEXT("GetComponentHierarchy should fail for non-existent blueprint"), HierarchyResult.IsFailure());

	// Test empty parameter validation
	UnrealMCP::FComponentPropertiesParams EmptyBlueprintParams;
	EmptyBlueprintParams.BlueprintName = TEXT("");
	EmptyBlueprintParams.ComponentName = TEXT("SomeComponent");

	const auto EmptyBlueprintResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(
		EmptyBlueprintParams);
	TestTrue(TEXT("Should fail for empty blueprint name"), EmptyBlueprintResult.IsFailure());

	UnrealMCP::FComponentPropertiesParams EmptyComponentParams;
	EmptyComponentParams.BlueprintName = TEXT("SomeBlueprint");
	EmptyComponentParams.ComponentName = TEXT("");

	const auto EmptyComponentResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(
		EmptyComponentParams);
	TestTrue(TEXT("Should fail for empty component name"), EmptyComponentResult.IsFailure());

	// Test existence checks
	bool bExistsNonExistent = UnrealMCP::FBlueprintIntrospectionService::BlueprintExists(
		TEXT("NonExistentBlueprint_XYZ123"));
	TestFalse(TEXT("BlueprintExists should return false for non-existent"), bExistsNonExistent);

	bool bExistsEmpty = UnrealMCP::FBlueprintIntrospectionService::BlueprintExists(TEXT(""));
	TestFalse(TEXT("BlueprintExists should return false for empty name"), bExistsEmpty);

	// Test rename with empty new name
	UnrealMCP::FRenameComponentParams EmptyNewNameParams;
	EmptyNewNameParams.BlueprintName = TEXT("SomeBlueprint");
	EmptyNewNameParams.OldName = TEXT("OldName");
	EmptyNewNameParams.NewName = TEXT("");

	const auto EmptyNewNameResult = UnrealMCP::FBlueprintIntrospectionService::RenameComponent(EmptyNewNameParams);
	TestTrue(TEXT("Should fail for empty new name"), EmptyNewNameResult.IsFailure());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlueprintIntrospectionServiceComplexScenarioTest,
	"UnrealMCP.BlueprintIntrospection.ComplexScenario",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

auto FBlueprintIntrospectionServiceComplexScenarioTest::RunTest(const FString& Parameters) -> bool {
	// Test: Complex workflow with multiple operations on same blueprint

	// Create comprehensive test blueprint
	UnrealMCP::FBlueprintCreationParams Params;
	Params.Name = TEXT("ComplexTestBlueprint");
	Params.PackagePath = TEXT("/Game/Tests/Introspection/");
	Params.ParentClass = TEXT("Character");

	const auto CreateResult = UnrealMCP::FBlueprintCreationService::CreateBlueprint(Params);
	TestTrue(TEXT("Complex test blueprint should be created"), CreateResult.IsSuccess());

	// Add hierarchical component structure
	UnrealMCP::FComponentParams RootParams;
	RootParams.BlueprintName = TEXT("ComplexTestBlueprint");
	RootParams.ComponentName = TEXT("RootComp");
	RootParams.ComponentType = TEXT("SceneComponent");

	const auto RootResult = UnrealMCP::FBlueprintService::AddComponent(RootParams);
	TestTrue(TEXT("Root component should be added"), RootResult.IsSuccess());

	UnrealMCP::FComponentParams MeshParams;
	MeshParams.BlueprintName = TEXT("ComplexTestBlueprint");
	MeshParams.ComponentName = TEXT("BodyMesh");
	MeshParams.ComponentType = TEXT("StaticMeshComponent");

	const auto MeshResult = UnrealMCP::FBlueprintService::AddComponent(MeshParams);
	TestTrue(TEXT("Mesh component should be added"), MeshResult.IsSuccess());

	UnrealMCP::FComponentParams LightParams;
	LightParams.BlueprintName = TEXT("ComplexTestBlueprint");
	LightParams.ComponentName = TEXT("HeadLight");
	LightParams.ComponentType = TEXT("PointLightComponent");

	const auto LightResult = UnrealMCP::FBlueprintService::AddComponent(LightParams);
	TestTrue(TEXT("Light component should be added"), LightResult.IsSuccess());

	UnrealMCP::FComponentParams CollisionParams;
	CollisionParams.BlueprintName = TEXT("ComplexTestBlueprint");
	CollisionParams.ComponentName = TEXT("CollisionBox");
	CollisionParams.ComponentType = TEXT("BoxComponent");

	const auto CollisionResult = UnrealMCP::FBlueprintService::AddComponent(CollisionParams);
	TestTrue(TEXT("Collision component should be added"), CollisionResult.IsSuccess());

	UnrealMCP::FComponentParams AttachmentParams;
	AttachmentParams.BlueprintName = TEXT("ComplexTestBlueprint");
	AttachmentParams.ComponentName = TEXT("AttachmentPoint");
	AttachmentParams.ComponentType = TEXT("SceneComponent");

	const auto AttachmentResult = UnrealMCP::FBlueprintService::AddComponent(AttachmentParams);
	TestTrue(TEXT("Attachment component should be added"), AttachmentResult.IsSuccess());

	// Step 1: Get blueprint info and verify initial state
	TMap<FString, FString> BlueprintInfo;
	UnrealMCP::FVoidResult InfoResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintInfo(
		TEXT("ComplexTestBlueprint"),
		BlueprintInfo);
	TestTrue(TEXT("GetBlueprintInfo should succeed"), InfoResult.IsSuccess());
	TestTrue(TEXT("Should have multiple components"), FCString::Atoi(*BlueprintInfo[TEXT("num_components")]) >= 5);

	// Step 2: Get detailed variable information
	const auto VariablesResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintVariables(
		TEXT("ComplexTestBlueprint"));
	TestTrue(TEXT("GetBlueprintVariables should succeed"), VariablesResult.IsSuccess());

	// Step 3: Get component hierarchy
	UnrealMCP::FComponentHierarchyParams HierarchyParams;
	HierarchyParams.BlueprintName = TEXT("ComplexTestBlueprint");
	const auto HierarchyResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentHierarchy(HierarchyParams);
	TestTrue(TEXT("GetComponentHierarchy should succeed"), HierarchyResult.IsSuccess());
	TestTrue(TEXT("Should have root components"), HierarchyResult.GetValue().RootCount > 0);
	TestTrue(TEXT("Should have total components"), HierarchyResult.GetValue().TotalComponents >= 5);

	// Step 4: Get specific component properties
	UnrealMCP::FComponentPropertiesParams GetMeshParams;
	GetMeshParams.BlueprintName = TEXT("ComplexTestBlueprint");
	GetMeshParams.ComponentName = TEXT("BodyMesh");

	const auto GetMeshPropsResult = UnrealMCP::FBlueprintIntrospectionService::GetComponentProperties(GetMeshParams);
	TestTrue(TEXT("Get mesh properties should succeed"), GetMeshPropsResult.IsSuccess());

	const auto& MeshProps = GetMeshPropsResult.GetValue().Properties;
	TestEqual(TEXT("Mesh name should match"), MeshProps->GetStringField(TEXT("name")), TEXT("BodyMesh"));
	TestTrue(TEXT("Mesh should be StaticMeshComponent"),
	         MeshProps->GetStringField(TEXT("type")).Contains(TEXT("StaticMeshComponent")));

	// Step 5: Remove a component
	UnrealMCP::FRemoveComponentParams RemoveParams;
	RemoveParams.BlueprintName = TEXT("ComplexTestBlueprint");
	RemoveParams.ComponentName = TEXT("AttachmentPoint");

	const auto RemoveResult = UnrealMCP::FBlueprintIntrospectionService::RemoveComponent(RemoveParams);
	TestTrue(TEXT("Remove component should succeed"), RemoveResult.IsSuccess());

	// Step 6: Rename a component
	UnrealMCP::FRenameComponentParams RenameParams;
	RenameParams.BlueprintName = TEXT("ComplexTestBlueprint");
	RenameParams.OldName = TEXT("HeadLight");
	RenameParams.NewName = TEXT("SpotLight");

	const auto RenameResult = UnrealMCP::FBlueprintIntrospectionService::RenameComponent(RenameParams);
	TestTrue(TEXT("Rename component should succeed"), RenameResult.IsSuccess());

	// Step 7: Verify final state
	TArray<TMap<FString, FString>> FinalComponents;
	UnrealMCP::FVoidResult FinalComponentsResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintComponents(
		TEXT("ComplexTestBlueprint"),
		FinalComponents);
	TestTrue(TEXT("Get final components should succeed"), FinalComponentsResult.IsSuccess());

	// Verify removal and rename
	bool bFoundRemoved = false, bFoundRenamed = false, bFoundOthers = false;
	for (const TMap<FString, FString>& Component : FinalComponents) {
		const FString* Name = Component.Find(TEXT("name"));
		if (Name) {
			if (*Name == TEXT("AttachmentPoint"))
				bFoundRemoved = true;
			if (*Name == TEXT("SpotLight"))
				bFoundRenamed = true;
			if (*Name == TEXT("BodyMesh") || *Name == TEXT("CollisionBox") || *Name == TEXT("RootComp")) {
				bFoundOthers = true;
			}
		}
	}

	TestFalse(TEXT("Removed component should not be found"), bFoundRemoved);
	TestTrue(TEXT("Renamed component should be found"), bFoundRenamed);
	TestTrue(TEXT("Other components should still exist"), bFoundOthers);

	// Step 8: Get updated blueprint info
	TMap<FString, FString> UpdatedInfo;
	UnrealMCP::FVoidResult UpdatedInfoResult = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintInfo(
		TEXT("ComplexTestBlueprint"),
		UpdatedInfo);
	TestTrue(TEXT("Get updated blueprint info should succeed"), UpdatedInfoResult.IsSuccess());
	TestTrue(TEXT("Should have fewer components after removal"),
	         FCString::Atoi(*UpdatedInfo[TEXT("num_components")]) <= 4);

	return true;
}
