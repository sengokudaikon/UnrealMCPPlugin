#include "Core/MCPRegistry.h"
#include "Engine/Blueprint.h"
#include "Blueprint/UserWidget.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "UObject/UObjectIterator.h"
#include "K2Node.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"

namespace UnrealMCP {

	// Static member initialization
	TArray<UClass*> FMCPRegistry::ParentClassCache;
	TArray<UClass*> FMCPRegistry::ComponentTypeCache;
	TArray<UClass*> FMCPRegistry::WidgetTypeCache;
	bool FMCPRegistry::bRegistriesInitialized = false;

	// ============ Registry Initialization ============

	void FMCPRegistry::Initialize() {
		if (bRegistriesInitialized) {
			return;
		}

		BuildParentClassCache();
		BuildComponentTypeCache();
		BuildWidgetTypeCache();

		bRegistriesInitialized = true;
		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Registries initialized successfully"));
	}

	// ============ Parent Class Registry ============

	FVoidResult FMCPRegistry::GetSupportedParentClasses(TArray<FString>& OutClasses) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		OutClasses.Empty();
		for (const UClass* Class : ParentClassCache) {
			if (Class) {
				OutClasses.Add(Class->GetName());
			}
		}

		return FVoidResult::Success();
	}

	bool FMCPRegistry::IsValidParentClass(const FString& ClassName) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* ResolvedClass = ResolveClassName(ClassName, AActor::StaticClass());
		return ResolvedClass != nullptr;
	}

	FVoidResult FMCPRegistry::GetParentClassInfo(const FString& ClassName, TMap<FString, FString>& OutInfo) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* Class = ResolveClassName(ClassName, AActor::StaticClass());
		if (!Class) {
			return FVoidResult::Failure(FString::Printf(TEXT("Class '%s' not found"), *ClassName));
		}

		OutInfo.Empty();
		OutInfo.Add(TEXT("name"), Class->GetName());
		OutInfo.Add(TEXT("full_name"), Class->GetPathName());
		OutInfo.Add(TEXT("parent"), Class->GetSuperClass() ? Class->GetSuperClass()->GetName() : TEXT("None"));
		OutInfo.Add(TEXT("abstract"), Class->HasAnyClassFlags(CLASS_Abstract) ? TEXT("true") : TEXT("false"));
		OutInfo.Add(TEXT("deprecated"), Class->HasAnyClassFlags(CLASS_Deprecated) ? TEXT("true") : TEXT("false"));
		OutInfo.Add(TEXT("native"), Class->HasAnyClassFlags(CLASS_Native) ? TEXT("true") : TEXT("false"));

		// Get module/package info
		if (const UPackage* Package = Class->GetOutermost()) {
			OutInfo.Add(TEXT("module"), Package->GetName());
		}

		return FVoidResult::Success();
	}

	// ============ Component Type Registry ============

	FVoidResult FMCPRegistry::GetSupportedComponentTypes(TArray<FString>& OutComponentTypes) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		OutComponentTypes.Empty();
		for (const UClass* Class : ComponentTypeCache) {
			if (Class) {
				OutComponentTypes.Add(Class->GetName());
			}
		}

		return FVoidResult::Success();
	}

	bool FMCPRegistry::IsValidComponentType(const FString& ComponentType) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* ResolvedClass = ResolveClassName(ComponentType, UActorComponent::StaticClass());
		return ResolvedClass != nullptr;
	}

	FVoidResult FMCPRegistry::GetComponentTypeInfo(const FString& ComponentType, TMap<FString, FString>& OutInfo) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* Class = ResolveClassName(ComponentType, UActorComponent::StaticClass());
		if (!Class) {
			return FVoidResult::Failure(FString::Printf(TEXT("Component type '%s' not found"), *ComponentType));
		}

		OutInfo.Empty();
		OutInfo.Add(TEXT("name"), Class->GetName());
		OutInfo.Add(TEXT("full_name"), Class->GetPathName());
		OutInfo.Add(TEXT("parent"), Class->GetSuperClass() ? Class->GetSuperClass()->GetName() : TEXT("None"));
		OutInfo.Add(TEXT("abstract"), Class->HasAnyClassFlags(CLASS_Abstract) ? TEXT("true") : TEXT("false"));
		OutInfo.Add(TEXT("is_scene_component"),
		            Class->IsChildOf(USceneComponent::StaticClass()) ? TEXT("true") : TEXT("false"));

		return FVoidResult::Success();
	}

	// ============ API Method Registry ============

	FVoidResult FMCPRegistry::GetAvailableAPIMethods(TMap<FString, TArray<FString>>& OutMethods) {
		OutMethods.Empty();

		// Blueprint methods
		const TArray<FString> BlueprintMethods = {
			TEXT("create_blueprint"),
			TEXT("compile_blueprint"),
			TEXT("spawn_blueprint_actor"),
			TEXT("set_blueprint_property"),
			TEXT("set_pawn_properties"),
			TEXT("list_blueprints"),
			TEXT("get_blueprint_info"),
			TEXT("blueprint_exists"),
			TEXT("delete_blueprint"),
			TEXT("duplicate_blueprint"),
			TEXT("get_blueprint_path")
		};
		OutMethods.Add(TEXT("blueprint"), BlueprintMethods);

		// Component methods
		const TArray<FString> ComponentMethods = {
			TEXT("add_component_to_blueprint"),
			TEXT("set_static_mesh_properties"),
			TEXT("set_physics_properties"),
			TEXT("set_component_property"),
			TEXT("get_blueprint_components"),
			TEXT("get_component_properties"),
			TEXT("remove_component"),
			TEXT("rename_component")
		};
		OutMethods.Add(TEXT("component"), ComponentMethods);

		// Blueprint Graph methods
		const TArray<FString> GraphMethods = {
			TEXT("add_blueprint_event"),
			TEXT("add_blueprint_function_call"),
			TEXT("add_blueprint_variable"),
			TEXT("connect_blueprint_nodes"),
			TEXT("find_blueprint_nodes"),
			TEXT("add_blueprint_input_action_node"),
			TEXT("add_blueprint_self_reference"),
			TEXT("add_blueprint_get_self_component_reference"),
			TEXT("get_blueprint_graph_nodes"),
			TEXT("remove_node"),
			TEXT("get_node_info")
		};
		OutMethods.Add(TEXT("graph"), GraphMethods);

		// Actor methods
		const TArray<FString> ActorMethods = {
			TEXT("spawn_actor"),
			TEXT("delete_actor"),
			TEXT("get_actors_in_level"),
			TEXT("find_actors_by_name"),
			TEXT("get_actor_properties"),
			TEXT("set_actor_property"),
			TEXT("set_actor_transform")
		};
		OutMethods.Add(TEXT("actor"), ActorMethods);

		// Widget/UMG methods
		const TArray<FString> WidgetMethods = {
			TEXT("create_umg_widget_blueprint"),
			TEXT("add_text_block_to_widget"),
			TEXT("add_button_to_widget"),
			TEXT("bind_widget_event"),
			TEXT("set_text_block_binding"),
			TEXT("add_widget_to_viewport")
		};
		OutMethods.Add(TEXT("widget"), WidgetMethods);

		// Input methods
		const TArray<FString> InputMethods = {
			TEXT("create_enhanced_input_action"),
			TEXT("create_input_mapping_context"),
			TEXT("add_enhanced_input_mapping"),
			TEXT("remove_enhanced_input_mapping"),
			TEXT("apply_mapping_context"),
			TEXT("remove_mapping_context"),
			TEXT("clear_all_mapping_contexts"),
			TEXT("create_input_mapping")
		};
		OutMethods.Add(TEXT("input"), InputMethods);

		// Editor methods
		const TArray<FString> EditorMethods = {
			TEXT("take_screenshot"),
			TEXT("focus_viewport")
		};
		OutMethods.Add(TEXT("editor"), EditorMethods);

		// Registry methods
		const TArray<FString> RegistryMethods = {
			TEXT("get_supported_parent_classes"),
			TEXT("get_supported_component_types"),
			TEXT("get_supported_widget_types"),
			TEXT("get_supported_property_types"),
			TEXT("get_supported_node_types"),
			TEXT("get_available_api_methods")
		};
		OutMethods.Add(TEXT("registry"), RegistryMethods);

		return FVoidResult::Success();
	}

	FVoidResult FMCPRegistry::GetAPIMethodInfo(const FString& MethodName, TMap<FString, FString>& OutInfo) {
		// This is a simplified implementation. In a full version, you'd maintain
		// a comprehensive registry of method signatures and documentation.
		OutInfo.Empty();

		// Example for a few methods
		if (MethodName == TEXT("create_blueprint")) {
			OutInfo.Add(TEXT("name"), TEXT("create_blueprint"));
			OutInfo.Add(TEXT("description"), TEXT("Create a new Blueprint class"));
			OutInfo.Add(TEXT("parameters"), TEXT("name: string, parent_class: string"));
			OutInfo.Add(TEXT("returns"), TEXT("Blueprint object or error"));
			OutInfo.Add(TEXT("category"), TEXT("blueprint"));
		}
		else if (MethodName == TEXT("add_component_to_blueprint")) {
			OutInfo.Add(TEXT("name"), TEXT("add_component_to_blueprint"));
			OutInfo.Add(TEXT("description"), TEXT("Add a component to a Blueprint"));
			OutInfo.Add(TEXT("parameters"),
			            TEXT(
				            "blueprint_name: string, component_type: string, component_name: string, location: vector3, rotation: vector3, scale: vector3"));
			OutInfo.Add(TEXT("returns"), TEXT("Success or error"));
			OutInfo.Add(TEXT("category"), TEXT("component"));
		}
		else {
			return FVoidResult::Failure(FString::Printf(TEXT("Method '%s' not found in registry"), *MethodName));
		}

		return FVoidResult::Success();
	}

	// ============ Widget Type Registry ============

	FVoidResult FMCPRegistry::GetSupportedWidgetTypes(TArray<FString>& OutWidgetTypes) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		OutWidgetTypes.Empty();
		for (const UClass* Class : WidgetTypeCache) {
			if (Class) {
				OutWidgetTypes.Add(Class->GetName());
			}
		}

		return FVoidResult::Success();
	}

	bool FMCPRegistry::IsValidWidgetType(const FString& WidgetType) {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* ResolvedClass = ResolveClassName(WidgetType, UUserWidget::StaticClass());
		return ResolvedClass != nullptr;
	}

	// ============ Property Type Registry ============

	FVoidResult FMCPRegistry::GetSupportedPropertyTypes(TArray<FString>& OutPropertyTypes) {
		OutPropertyTypes.Empty();

		// Basic types
		OutPropertyTypes.Add(TEXT("Boolean"));
		OutPropertyTypes.Add(TEXT("Integer"));
		OutPropertyTypes.Add(TEXT("Float"));
		OutPropertyTypes.Add(TEXT("String"));
		OutPropertyTypes.Add(TEXT("Name"));
		OutPropertyTypes.Add(TEXT("Text"));

		// Math types
		OutPropertyTypes.Add(TEXT("Vector"));
		OutPropertyTypes.Add(TEXT("Vector2D"));
		OutPropertyTypes.Add(TEXT("Rotator"));
		OutPropertyTypes.Add(TEXT("Transform"));
		OutPropertyTypes.Add(TEXT("LinearColor"));
		OutPropertyTypes.Add(TEXT("Color"));

		// Object types
		OutPropertyTypes.Add(TEXT("Object"));
		OutPropertyTypes.Add(TEXT("Actor"));
		OutPropertyTypes.Add(TEXT("Class"));

		// Container types
		OutPropertyTypes.Add(TEXT("Array"));
		OutPropertyTypes.Add(TEXT("Map"));
		OutPropertyTypes.Add(TEXT("Set"));

		return FVoidResult::Success();
	}

	bool FMCPRegistry::IsValidPropertyType(const FString& PropertyType) {
		TArray<FString> SupportedTypes;
		GetSupportedPropertyTypes(SupportedTypes);
		return SupportedTypes.Contains(PropertyType);
	}

	// ============ Node Type Registry ============

	FVoidResult FMCPRegistry::GetSupportedNodeTypes(TArray<FString>& OutNodeTypes) {
		OutNodeTypes.Empty();

		OutNodeTypes.Add(TEXT("Event"));
		OutNodeTypes.Add(TEXT("FunctionCall"));
		OutNodeTypes.Add(TEXT("VariableGet"));
		OutNodeTypes.Add(TEXT("VariableSet"));
		OutNodeTypes.Add(TEXT("SelfReference"));
		OutNodeTypes.Add(TEXT("ComponentReference"));
		OutNodeTypes.Add(TEXT("InputAction"));
		OutNodeTypes.Add(TEXT("Branch"));
		OutNodeTypes.Add(TEXT("Sequence"));
		OutNodeTypes.Add(TEXT("ForEachLoop"));
		OutNodeTypes.Add(TEXT("WhileLoop"));
		OutNodeTypes.Add(TEXT("Delay"));
		OutNodeTypes.Add(TEXT("Timeline"));
		OutNodeTypes.Add(TEXT("CustomEvent"));

		return FVoidResult::Success();
	}

	FVoidResult FMCPRegistry::GetNodeTypeInfo(const FString& NodeType, TMap<FString, FString>& OutInfo) {
		OutInfo.Empty();

		if (NodeType == TEXT("Event")) {
			OutInfo.Add(TEXT("name"), TEXT("Event"));
			OutInfo.Add(TEXT("description"), TEXT("Blueprint event node (e.g., BeginPlay, Tick)"));
			OutInfo.Add(TEXT("category"), TEXT("Events"));
		}
		else if (NodeType == TEXT("FunctionCall")) {
			OutInfo.Add(TEXT("name"), TEXT("FunctionCall"));
			OutInfo.Add(TEXT("description"), TEXT("Call a function"));
			OutInfo.Add(TEXT("category"), TEXT("Functions"));
		}
		else if (NodeType == TEXT("Branch")) {
			OutInfo.Add(TEXT("name"), TEXT("Branch"));
			OutInfo.Add(TEXT("description"), TEXT("Conditional branching (if/else)"));
			OutInfo.Add(TEXT("category"), TEXT("Flow Control"));
		}
		else {
			return FVoidResult::Failure(FString::Printf(TEXT("Node type '%s' not found"), *NodeType));
		}

		return FVoidResult::Success();
	}

	// ============ Internal Helper Methods ============

	void FMCPRegistry::BuildParentClassCache() {
		ParentClassCache.Empty();

		// Iterate through all UClasses derived from AActor
		for (TObjectIterator<UClass> It; It; ++It) {
			UClass* Class = *It;

			// Must be a child of AActor
			if (!Class->IsChildOf(AActor::StaticClass())) {
				continue;
			}

			// Skip classes that should be excluded
			if (ShouldExcludeClass(Class)) {
				continue;
			}

			// Skip Blueprint-generated classes (we only want native/editor classes as parents)
			if (Class->ClassGeneratedBy != nullptr) {
				continue;
			}

			ParentClassCache.Add(Class);
		}

		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Found %d valid parent classes"), ParentClassCache.Num());
	}

	void FMCPRegistry::BuildComponentTypeCache() {
		ComponentTypeCache.Empty();

		// Iterate through all UClasses derived from UActorComponent
		for (TObjectIterator<UClass> It; It; ++It) {
			UClass* Class = *It;

			// Must be a child of UActorComponent
			if (!Class->IsChildOf(UActorComponent::StaticClass())) {
				continue;
			}

			// Skip classes that should be excluded
			if (ShouldExcludeClass(Class)) {
				continue;
			}

			// Skip Blueprint-generated classes
			if (Class->ClassGeneratedBy != nullptr) {
				continue;
			}

			ComponentTypeCache.Add(Class);
		}

		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Found %d valid component types"), ComponentTypeCache.Num());
	}

	void FMCPRegistry::BuildWidgetTypeCache() {
		WidgetTypeCache.Empty();

		// Iterate through all UClasses derived from UUserWidget
		for (TObjectIterator<UClass> It; It; ++It) {
			UClass* Class = *It;

			// Must be a child of UUserWidget
			if (!Class->IsChildOf(UUserWidget::StaticClass())) {
				continue;
			}

			// Skip classes that should be excluded
			if (ShouldExcludeClass(Class)) {
				continue;
			}

			// Skip Blueprint-generated classes
			if (Class->ClassGeneratedBy != nullptr) {
				continue;
			}

			WidgetTypeCache.Add(Class);
		}

		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Found %d valid widget types"), WidgetTypeCache.Num());
	}

	UClass* FMCPRegistry::ResolveClassName(const FString& ClassName, const UClass* BaseClass) {
		if (ClassName.IsEmpty()) {
			return nullptr;
		}

		// Try exact name first
		UClass* FoundClass = FindFirstObject<UClass>(*ClassName, EFindFirstObjectOptions::NativeFirst);
		if (FoundClass && (!BaseClass || FoundClass->IsChildOf(BaseClass))) {
			return FoundClass;
		}

		// Try with 'A' prefix (for actors)
		if (!ClassName.StartsWith(TEXT("A"))) {
			const FString ActorClassName = FString::Printf(TEXT("A%s"), *ClassName);
			FoundClass = FindFirstObject<UClass>(*ActorClassName, EFindFirstObjectOptions::NativeFirst);
			if (FoundClass && (!BaseClass || FoundClass->IsChildOf(BaseClass))) {
				return FoundClass;
			}
		}

		// Try with 'U' prefix (for objects/components)
		if (!ClassName.StartsWith(TEXT("U"))) {
			const FString ObjectClassName = FString::Printf(TEXT("U%s"), *ClassName);
			FoundClass = FindFirstObject<UClass>(*ObjectClassName, EFindFirstObjectOptions::NativeFirst);
			if (FoundClass && (!BaseClass || FoundClass->IsChildOf(BaseClass))) {
				return FoundClass;
			}
		}

		// Try with "Component" suffix for component types
		if (BaseClass && BaseClass->IsChildOf(UActorComponent::StaticClass())) {
			if (!ClassName.EndsWith(TEXT("Component"))) {
				const FString ComponentClassName = FString::Printf(TEXT("%sComponent"), *ClassName);
				FoundClass = FindFirstObject<UClass>(*ComponentClassName, EFindFirstObjectOptions::NativeFirst);
				if (FoundClass && FoundClass->IsChildOf(BaseClass)) {
					return FoundClass;
				}

				// Try with U prefix and Component suffix
				const FString UComponentClassName = FString::Printf(TEXT("U%sComponent"), *ClassName);
				FoundClass = FindFirstObject<UClass>(*UComponentClassName, EFindFirstObjectOptions::NativeFirst);
				if (FoundClass && FoundClass->IsChildOf(BaseClass)) {
					return FoundClass;
				}
			}
		}

		return nullptr;
	}

	bool FMCPRegistry::ShouldExcludeClass(const UClass* Class) {
		if (!Class) {
			return true;
		}

		// Exclude abstract classes
		if (Class->HasAnyClassFlags(CLASS_Abstract)) {
			return true;
		}

		// Exclude deprecated classes
		if (Class->HasAnyClassFlags(CLASS_Deprecated)) {
			return true;
		}

		// Exclude hidden classes
		if (Class->HasAnyClassFlags(CLASS_Hidden)) {
			return true;
		}

		// Exclude editor-only classes in shipping builds
#if !WITH_EDITOR
		if (Class->HasAnyClassFlags(CLASS_EditorOnly)) {
			return true;
		}
#endif

		return false;
	}

} // namespace UnrealMCP
