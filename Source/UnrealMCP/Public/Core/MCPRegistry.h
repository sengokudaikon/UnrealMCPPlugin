#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"

namespace UnrealMCP {

	/**
	 * Central registry for UnrealMCP capabilities and supported types.
	 * Provides queryable information about what's available in the API.
	 */
	class UNREALMCP_API FMCPRegistry {
	public:
		/**
		 * Initialize all registries. Called on module startup.
		 */
		static void Initialize();

		/**
		 * Get all supported parent classes for Blueprint creation.
		 * Uses Unreal's reflection system to find all valid Actor-derived classes.
		 *
		 * @param OutClasses Array to populate with class names
		 * @return Success if classes were retrieved
		 */
		static auto GetSupportedParentClasses(TArray<FString>& OutClasses) -> FVoidResult;

		/**
		 * Check if a parent class is valid for Blueprint creation.
		 *
		 * @param ClassName Name of the class to validate
		 * @return True if the class can be used as a Blueprint parent
		 */
		static bool IsValidParentClass(const FString& ClassName);

		/**
		 * Get detailed information about a parent class.
		 *
		 * @param ClassName Name of the class
		 * @param OutInfo Map with class metadata (module, category, abstract, etc.)
		 * @return Success if class info was retrieved
		 */
		static auto GetParentClassInfo(const FString& ClassName, TMap<FString, FString>& OutInfo) -> FVoidResult;

		/**
		 * Get all supported component types.
		 * Uses reflection to find all ActorComponent-derived classes.
		 *
		 * @param OutComponentTypes Array to populate with component type names
		 * @return Success if component types were retrieved
		 */
		static auto GetSupportedComponentTypes(TArray<FString>& OutComponentTypes) -> FVoidResult;

		/**
		 * Check if a component type is valid.
		 *
		 * @param ComponentType Name of the component type to validate
		 * @return True if the component type is valid
		 */
		static bool IsValidComponentType(const FString& ComponentType);

		/**
		 * Get detailed information about a component type.
		 *
		 * @param ComponentType Name of the component type
		 * @param OutInfo Map with component metadata (category, description, etc.)
		 * @return Success if component info was retrieved
		 */
		static auto GetComponentTypeInfo(const FString& ComponentType, TMap<FString, FString>& OutInfo) -> FVoidResult;

		/**
		 * Get all available API methods organized by category.
		 *
		 * @param OutMethods Map of category -> array of method names
		 * @return Success if methods were retrieved
		 */
		static auto GetAvailableAPIMethods(TMap<FString, TArray<FString>>& OutMethods) -> FVoidResult;

		/**
		 * Get detailed information about an API method.
		 *
		 * @param MethodName Name of the method
		 * @param OutInfo Map with method metadata (description, parameters, return type)
		 * @return Success if method info was retrieved
		 */
		static auto GetAPIMethodInfo(const FString& MethodName, TMap<FString, FString>& OutInfo) -> FVoidResult;

		/**
		 * Get all supported UMG widget types.
		 *
		 * @param OutWidgetTypes Array to populate with widget type names
		 * @return Success if widget types were retrieved
		 */
		static auto GetSupportedWidgetTypes(TArray<FString>& OutWidgetTypes) -> FVoidResult;

		/**
		 * Check if a widget type is valid for UMG.
		 *
		 * @param WidgetType Name of the widget type to validate
		 * @return True if the widget type is valid
		 */
		static bool IsValidWidgetType(const FString& WidgetType);

		/**
		 * Get all supported property types for Blueprint variables.
		 *
		 * @param OutPropertyTypes Array to populate with property type names
		 * @return Success if property types were retrieved
		 */
		static auto GetSupportedPropertyTypes(TArray<FString>& OutPropertyTypes) -> FVoidResult;

		/**
		 * Check if a property type is valid.
		 *
		 * @param PropertyType Name of the property type to validate
		 * @return True if the property type is valid
		 */
		static bool IsValidPropertyType(const FString& PropertyType);

		/**
		 * Get all supported Blueprint node types.
		 *
		 * @param OutNodeTypes Array to populate with node type names
		 * @return Success if node types were retrieved
		 */
		static auto GetSupportedNodeTypes(TArray<FString>& OutNodeTypes) -> FVoidResult;

		/**
		 * Get detailed information about a node type.
		 *
		 * @param NodeType Name of the node type
		 * @param OutInfo Map with node metadata (category, pins, etc.)
		 * @return Success if node info was retrieved
		 */
		static auto GetNodeTypeInfo(const FString& NodeType, TMap<FString, FString>& OutInfo) -> FVoidResult;

	private:
		static TArray<UClass*> ParentClassCache;
		static TArray<UClass*> ComponentTypeCache;
		static TArray<UClass*> WidgetTypeCache;
		static bool bRegistriesInitialized;

		static void BuildParentClassCache();
		static void BuildComponentTypeCache();
		static void BuildWidgetTypeCache();
		static auto ResolveClassName(const FString& ClassName, const UClass* BaseClass = nullptr) -> UClass*;
		static bool ShouldExcludeClass(const UClass* Class);
	};

} // namespace UnrealMCP
