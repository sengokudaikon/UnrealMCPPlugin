#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"

class UBlueprint;

namespace UnrealMCP {

	/**
	 * Service for Blueprint introspection and querying operations.
	 * Provides methods to list, search, and get information about blueprints.
	 */
	class UNREALMCP_API FBlueprintIntrospectionService {
	public:
		/**
		 * List all blueprints in a directory.
		 *
		 * @param Path Directory path to search (defaults to /Game/)
		 * @param bRecursive Whether to search subdirectories
		 * @param OutBlueprints Array to populate with blueprint paths
		 * @return Success if blueprints were listed
		 */
		static auto ListBlueprints(
			const FString& Path,
			bool bRecursive,
			TArray<FString>& OutBlueprints
		) -> FVoidResult;

		/**
		 * Check if a blueprint exists.
		 *
		 * @param BlueprintName Name or path of the blueprint
		 * @return True if the blueprint exists
		 */
		static bool BlueprintExists(const FString& BlueprintName);

		/**
		 * Get comprehensive information about a blueprint.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param OutInfo Map with blueprint metadata
		 * @return Success if info was retrieved
		 */
		static auto GetBlueprintInfo(
			const FString& BlueprintName,
			TMap<FString, FString>& OutInfo
		) -> FVoidResult;

		/**
		 * Get all components in a blueprint.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param OutComponents Array of component info maps
		 * @return Success if components were retrieved
		 */
		static auto GetBlueprintComponents(
			const FString& BlueprintName,
			TArray<TMap<FString, FString>>& OutComponents
		) -> FVoidResult;

		/**
		 * Get all variables in a blueprint.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param OutVariables Array of variable info maps
		 * @return Success if variables were retrieved
		 */
		static auto GetBlueprintVariables(
			const FString& BlueprintName,
			TArray<TMap<FString, FString>>& OutVariables
		) -> FVoidResult;

		/**
		 * Get full path for a blueprint.
		 *
		 * @param BlueprintName Short name or full path
		 * @return Full asset path or empty string if not found
		 */
		static auto GetBlueprintPath(const FString& BlueprintName) -> FString;

		/**
		 * Get component hierarchy for a blueprint.
		 *
		 * @param Params Parameters containing blueprint name
		 * @return Component hierarchy result or error
		 */
		static auto GetComponentHierarchy(const FComponentHierarchyParams& Params) -> TResult<FComponentHierarchyResult>;

	private:
		/**
		 * Find a blueprint by name or path.
		 *
		 * @param BlueprintName Name or path to search for
		 * @return Found blueprint or nullptr
		 */
		static auto FindBlueprint(const FString& BlueprintName) -> UBlueprint*;

		/**
		 * Resolve a blueprint path from a short name.
		 *
		 * @param BlueprintName Short name
		 * @return Full path
		 */
		static auto ResolveBlueprintPath(const FString& BlueprintName) -> FString;

		/**
		 * Build hierarchy node JSON from SCS node.
		 *
		 * @param Node SCS node to convert
		 * @return JSON object representing the node
		 */
		static auto BuildHierarchyNode(const USCS_Node* Node) -> TSharedPtr<FJsonObject>;
	};

} // namespace UnrealMCP
