#include "Core/MCPRegistry.h"
#include "Core/ErrorTypes.h"
#include "K2Node.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Blueprint/UserWidget.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraActor.h"
#include "Engine/DecalActor.h"
#include "GameFramework/WorldSettings.h"
#include "UObject/UObjectIterator.h"

namespace UnrealMCP {

	// Static member initialization
	TArray<UClass*>* FMCPRegistry::ParentClassCache = nullptr;
	TArray<UClass*>* FMCPRegistry::ComponentTypeCache = nullptr;
	TArray<UClass*>* FMCPRegistry::WidgetTypeCache = nullptr;
	bool FMCPRegistry::bRegistriesInitialized = false;

	// ============ Registry Initialization ============

	auto FMCPRegistry::Initialize() -> void {
		if (bRegistriesInitialized) {
			return;
		}

		// Allocate cache arrays
		ParentClassCache = new TArray<UClass*>();
		ComponentTypeCache = new TArray<UClass*>();
		WidgetTypeCache = new TArray<UClass*>();

		BuildParentClassCache();
		BuildComponentTypeCache();
		BuildWidgetTypeCache();

		bRegistriesInitialized = true;
		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Registries initialized successfully"));
	}

	// ============ Parent Class Registry ============

	auto FMCPRegistry::GetSupportedParentClasses(TArray<FString>& OutClasses) -> FVoidResult {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		OutClasses.Empty();
		for (const UClass* Class : *ParentClassCache) {
			if (Class) {
				OutClasses.Add(Class->GetName());
			}
		}

		return FVoidResult::Success();
	}

	auto FMCPRegistry::IsValidParentClass(const FString& ClassName) -> bool {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* ResolvedClass = ResolveClassName(ClassName, AActor::StaticClass());
		return ResolvedClass != nullptr;
	}

	auto FMCPRegistry::GetParentClassInfo(const FString& ClassName, TMap<FString, FString>& OutInfo) -> FVoidResult {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* Class = ResolveClassName(ClassName, AActor::StaticClass());
		if (!Class) {
			return FVoidResult::Failure(EErrorCode::InvalidParentClass, FString::Printf(TEXT("Class '%s' not found"), *ClassName));
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

	auto FMCPRegistry::GetSupportedComponentTypes(TArray<FString>& OutComponentTypes) -> FVoidResult {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		OutComponentTypes.Empty();
		for (const UClass* Class : *ComponentTypeCache) {
			if (Class) {
				OutComponentTypes.Add(Class->GetName());
			}
		}

		return FVoidResult::Success();
	}

	auto FMCPRegistry::IsValidComponentType(const FString& ComponentType) -> bool {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* ResolvedClass = ResolveClassName(ComponentType, UActorComponent::StaticClass());
		return ResolvedClass != nullptr;
	}

	auto FMCPRegistry::GetComponentTypeInfo(const FString& ComponentType,
	                                        TMap<FString, FString>& OutInfo) -> FVoidResult {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* Class = ResolveClassName(ComponentType, UActorComponent::StaticClass());
		if (!Class) {
			return FVoidResult::Failure(EErrorCode::InvalidComponentType, FString::Printf(TEXT("Component type '%s' not found"), *ComponentType));
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

	auto FMCPRegistry::GetAvailableAPIMethods(TMap<FString, TArray<FString>>& OutMethods) -> FVoidResult {
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
			TEXT("get_blueprint_variables"),
			TEXT("get_blueprint_functions"),
			TEXT("blueprint_exists"),
			TEXT("delete_blueprint"),
			TEXT("duplicate_blueprint"),
			TEXT("get_blueprint_path"),
			TEXT("add_function"),
			TEXT("remove_function"),
			TEXT("add_function_parameter"),
			TEXT("set_function_return_type"),
			TEXT("set_function_metadata"),
			TEXT("remove_variable"),
			TEXT("set_variable_default_value"),
			TEXT("set_variable_metadata"),
			TEXT("rename_variable")
		};
		OutMethods.Add(TEXT("blueprint"), BlueprintMethods);

		// Component methods
		const TArray<FString> ComponentMethods = {
			TEXT("add_component_to_blueprint"),
			TEXT("set_static_mesh_properties"),
			TEXT("set_physics_properties"),
			TEXT("set_component_property"),
			TEXT("set_component_transform"),
			TEXT("get_blueprint_components"),
			TEXT("get_component_properties"),
			TEXT("get_component_hierarchy"),
			TEXT("remove_component"),
			TEXT("rename_component")
		};
		OutMethods.Add(TEXT("component"), ComponentMethods);

		// Blueprint Graph methods
		const TArray<FString> GraphMethods = {
			TEXT("add_blueprint_event_node"),
			TEXT("add_blueprint_function_node"),
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
			TEXT("create_player_controller_in_editor"),
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

	auto FMCPRegistry::GetAPIMethodInfo(const FString& MethodName, TMap<FString, FString>& OutInfo) -> FVoidResult {
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
			return FVoidResult::Failure(EErrorCode::FunctionNotFound, FString::Printf(TEXT("Method '%s' not found in registry"), *MethodName));
		}

		return FVoidResult::Success();
	}

	// ============ Widget Type Registry ============

	auto FMCPRegistry::GetSupportedWidgetTypes(TArray<FString>& OutWidgetTypes) -> FVoidResult {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		OutWidgetTypes.Empty();
		for (const UClass* Class : *WidgetTypeCache) {
			if (Class) {
				OutWidgetTypes.Add(Class->GetName());
			}
		}

		return FVoidResult::Success();
	}

	auto FMCPRegistry::IsValidWidgetType(const FString& WidgetType) -> bool {
		if (!bRegistriesInitialized) {
			Initialize();
		}

		const UClass* ResolvedClass = ResolveClassName(WidgetType, UUserWidget::StaticClass());
		return ResolvedClass != nullptr;
	}

	// ============ Property Type Registry ============

	auto FMCPRegistry::GetSupportedPropertyTypes(TArray<FString>& OutPropertyTypes) -> FVoidResult {
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

	auto FMCPRegistry::IsValidPropertyType(const FString& PropertyType) -> bool {
		TArray<FString> SupportedTypes;
		GetSupportedPropertyTypes(SupportedTypes);
		return SupportedTypes.Contains(PropertyType);
	}

	// ============ Node Type Registry ============

	auto FMCPRegistry::GetSupportedNodeTypes(TArray<FString>& OutNodeTypes) -> FVoidResult {
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

	auto FMCPRegistry::GetNodeTypeInfo(const FString& NodeType, TMap<FString, FString>& OutInfo) -> FVoidResult {
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
			return FVoidResult::Failure(EErrorCode::NodeNotFound, FString::Printf(TEXT("Node type '%s' not found"), *NodeType));
		}

		return FVoidResult::Success();
	}

	// ============ Internal Helper Methods ============

	auto FMCPRegistry::BuildParentClassCache() -> void {
		ParentClassCache->Empty();
		
		TArray<FString> CoreModules = {
			TEXT("Engine"),
			TEXT("CoreUObject"),
			TEXT("GameFramework"),
			TEXT("EnhancedInput"),
			TEXT("Camera"),
			TEXT("GameplayTags"),
			TEXT("GameplayTasks"),
			TEXT("AIModule"),
			TEXT("Slate"),
			TEXT("UMG")
		};

		TArray<UClass*> CommonActorClasses = {
			AActor::StaticClass(),
			APawn::StaticClass(),
			ACharacter::StaticClass(),
			APlayerController::StaticClass(),
			AController::StaticClass(),
			AGameModeBase::StaticClass(),
			AGameStateBase::StaticClass(),
			APlayerState::StaticClass(),
			ACameraActor::StaticClass(),
			ADecalActor::StaticClass(),
			AWorldSettings::StaticClass()
		};

		for (UClass* Class : CommonActorClasses) {
			if (Class && !ShouldExcludeClass(Class)) {
				ParentClassCache->AddUnique(Class);
			}
		}
		
		if (ParentClassCache->Num() < 50) {
			UE_LOG(LogTemp, Warning, TEXT("UnrealMCP: Performing comprehensive class scan - this may take a moment..."));

			int32 ProcessedClasses = 0;
			for (TObjectIterator<UClass> It; It; ++It) {
				UClass* Class = *It;
				ProcessedClasses++;

				if (ProcessedClasses % 1000 == 0) {
					UE_LOG(LogTemp, VeryVerbose, TEXT("UnrealMCP: Processed %d classes..."), ProcessedClasses);
				}

				if (!Class->IsChildOf(AActor::StaticClass())) {
					continue;
				}

				if (ShouldExcludeClass(Class)) {
					continue;
				}

				if (Class->ClassGeneratedBy != nullptr) {
					continue;
				}

				ParentClassCache->AddUnique(Class);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Found %d valid parent classes"), ParentClassCache->Num());
	}

	auto FMCPRegistry::BuildComponentTypeCache() -> void {
		ComponentTypeCache->Empty();

		TArray<UClass*> CommonComponentClasses = {
			USceneComponent::StaticClass(),
			UActorComponent::StaticClass(),
			UStaticMeshComponent::StaticClass(),
			USkeletalMeshComponent::StaticClass(),
			UCameraComponent::StaticClass(),
			UCharacterMovementComponent::StaticClass(),
			UCapsuleComponent::StaticClass(),
			UBoxComponent::StaticClass(),
			USphereComponent::StaticClass(),
			UAudioComponent::StaticClass(),
			UParticleSystemComponent::StaticClass(),
			ULightComponent::StaticClass(),
			UPointLightComponent::StaticClass(),
			USpotLightComponent::StaticClass(),
			UDirectionalLightComponent::StaticClass(),
			USpringArmComponent::StaticClass(),
			UTimelineComponent::StaticClass(),
			UArrowComponent::StaticClass(),
			UBillboardComponent::StaticClass()
		};

		for (UClass* Class : CommonComponentClasses) {
			if (Class && !ShouldExcludeClass(Class)) {
				ComponentTypeCache->AddUnique(Class);
			}
		}

		if (ComponentTypeCache->Num() < 30) {
			UE_LOG(LogTemp, Warning, TEXT("UnrealMCP: Performing comprehensive component scan - this may take a moment..."));

			int32 ProcessedClasses = 0;
			for (TObjectIterator<UClass> It; It; ++It) {
				UClass* Class = *It;
				ProcessedClasses++;

				if (ProcessedClasses % 1000 == 0) {
					UE_LOG(LogTemp, VeryVerbose, TEXT("UnrealMCP: Processed %d component classes..."), ProcessedClasses);
				}

				if (!Class->IsChildOf(UActorComponent::StaticClass())) {
					continue;
				}

				if (ShouldExcludeClass(Class)) {
					continue;
				}

				if (Class->ClassGeneratedBy != nullptr) {
					continue;
				}

				ComponentTypeCache->AddUnique(Class);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Found %d valid component types"), ComponentTypeCache->Num());
	}

	auto FMCPRegistry::BuildWidgetTypeCache() -> void {
		WidgetTypeCache->Empty();

		TArray<UClass*> CommonWidgetClasses = {
			UUserWidget::StaticClass()
		};

		for (UClass* Class : CommonWidgetClasses) {
			if (Class && !ShouldExcludeClass(Class)) {
				WidgetTypeCache->AddUnique(Class);
			}
		}

		// For widgets, we typically don't need comprehensive scanning since most widgets are Blueprint-based
		// Only scan if we explicitly need more widget types
		if (WidgetTypeCache->Num() < 2) {
			UE_LOG(LogTemp, Warning, TEXT("UnrealMCP: Performing comprehensive widget scan - this may take a moment..."));

			int32 ProcessedClasses = 0;
			for (TObjectIterator<UClass> It; It; ++It) {
				UClass* Class = *It;
				ProcessedClasses++;

				if (ProcessedClasses % 1000 == 0) {
					UE_LOG(LogTemp, VeryVerbose, TEXT("UnrealMCP: Processed %d widget classes..."), ProcessedClasses);
				}

				if (!Class->IsChildOf(UUserWidget::StaticClass())) {
					continue;
				}

				if (ShouldExcludeClass(Class)) {
					continue;
				}

				if (Class->ClassGeneratedBy != nullptr) {
					continue;
				}

				WidgetTypeCache->AddUnique(Class);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("UnrealMCP: Found %d valid widget types"), WidgetTypeCache->Num());
	}

	auto FMCPRegistry::ResolveClassName(const FString& ClassName, const UClass* BaseClass) -> UClass* {
		if (ClassName.IsEmpty()) {
			return nullptr;
		}

		UClass* FoundClass = FindFirstObject<UClass>(*ClassName, EFindFirstObjectOptions::NativeFirst);
		if (FoundClass && (!BaseClass || FoundClass->IsChildOf(BaseClass))) {
			return FoundClass;
		}

		if (!ClassName.StartsWith(TEXT("A"))) {
			const FString ActorClassName = FString::Printf(TEXT("A%s"), *ClassName);
			FoundClass = FindFirstObject<UClass>(*ActorClassName, EFindFirstObjectOptions::NativeFirst);
			if (FoundClass && (!BaseClass || FoundClass->IsChildOf(BaseClass))) {
				return FoundClass;
			}
		}

		if (!ClassName.StartsWith(TEXT("U"))) {
			const FString ObjectClassName = FString::Printf(TEXT("U%s"), *ClassName);
			FoundClass = FindFirstObject<UClass>(*ObjectClassName, EFindFirstObjectOptions::NativeFirst);
			if (FoundClass && (!BaseClass || FoundClass->IsChildOf(BaseClass))) {
				return FoundClass;
			}
		}

		if (BaseClass && BaseClass->IsChildOf(UActorComponent::StaticClass())) {
			if (!ClassName.EndsWith(TEXT("Component"))) {
				const FString ComponentClassName = FString::Printf(TEXT("%sComponent"), *ClassName);
				FoundClass = FindFirstObject<UClass>(*ComponentClassName, EFindFirstObjectOptions::NativeFirst);
				if (FoundClass && FoundClass->IsChildOf(BaseClass)) {
					return FoundClass;
				}

				const FString UComponentClassName = FString::Printf(TEXT("U%sComponent"), *ClassName);
				FoundClass = FindFirstObject<UClass>(*UComponentClassName, EFindFirstObjectOptions::NativeFirst);
				if (FoundClass && FoundClass->IsChildOf(BaseClass)) {
					return FoundClass;
				}
			}
		}

		return nullptr;
	}

	auto FMCPRegistry::ShouldExcludeClass(const UClass* Class) -> bool {
		if (!Class) {
			return true;
		}

		if (Class->HasAnyClassFlags(CLASS_Abstract)) {
			return true;
		}

		if (Class->HasAnyClassFlags(CLASS_Deprecated)) {
			return true;
		}

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

}
