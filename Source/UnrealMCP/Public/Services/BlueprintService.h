#pragma once

#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "CoreMinimal.h"

class UBlueprint;
class AActor;
class USCS_Node;
class UPrimitiveComponent;

namespace UnrealMCP {
	/**
	 * Service for blueprint runtime operations and manipulation.
	 *
	 * Handles operations that affect blueprint instances and their component
	 * hierarchies at runtime. This service is focused on manipulation of existing
	 * blueprints, not their creation (see FBlueprintCreationService for creation).
	 *
	 * All methods are static for consistency. To get JSON responses, commands
	 * should handle the serialization at the command handler level.
	 */
	class UNREALMCP_API FBlueprintService {
	public:
		// ============ Actor Spawning ============

		/**
		 * Spawn an actor instance from a blueprint.
		 *
		 * @param Params Blueprint spawn parameters (name, location, rotation)
		 * @return The spawned actor instance or an error
		 */
		static TResult<AActor*> SpawnActorBlueprint(const FBlueprintSpawnParams& Params);

		// ============ Component Operations ============

		/**
		 * Add a component to a blueprint's construction script.
		 *
		 * Creates a new component node in the blueprint's Simple Construction Script
		 * and applies transform/mesh properties. The blueprint is compiled after addition.
		 *
		 * @param Params Component configuration (type, name, transform, mesh)
		 * @return The modified blueprint or an error
		 */
		static TResult<UBlueprint*> AddComponent(const FComponentParams& Params);

		/**
		 * Set a property on a blueprint's component.
		 *
		 * Modifies a component template's property within the blueprint's
		 * construction script. Blueprint is marked as modified.
		 *
		 * @param BlueprintName Name of the blueprint to modify
		 * @param ComponentName Name of the component within the blueprint
		 * @param PropertyParams Property to set and its value
		 * @return Success or an error
		 */
		static FVoidResult SetComponentProperty(
			const FString& BlueprintName,
			const FString& ComponentName,
			const FPropertyParams& PropertyParams
		);

		/**
		 * Set physics properties on a primitive component.
		 *
		 * Applies simulation, mass, damping, and gravity settings to a
		 * component within a blueprint. Blueprint is marked as modified.
		 *
		 * @param Params Physics configuration for the component
		 * @return Success or an error
		 */
		static FVoidResult SetPhysicsProperties(const FPhysicsParams& Params);

		/**
		 * Set static mesh and optional material on a component.
		 *
		 * Loads and assigns a static mesh asset and optional material to a
		 * StaticMeshComponent within a blueprint. Blueprint is marked as modified.
		 *
		 * @param BlueprintName Name of the blueprint to modify
		 * @param ComponentName Name of the component to modify
		 * @param StaticMesh Path to the mesh asset to load
		 * @param Material Optional path to material asset to load
		 * @return Success or an error
		 */
		static FVoidResult SetStaticMeshProperties(
			const FString& BlueprintName,
			const FString& ComponentName,
			const FString& StaticMesh,
			const TOptional<FString>& Material = TOptional<FString>()
		);

		/**
		 * Set transform properties on a scene component.
		 *
		 * Applies location, rotation, and/or scale settings to a
		 * SceneComponent within a blueprint. Blueprint is marked as modified
		 * and compiled after changes.
		 *
		 * @param Params Transform configuration for the component
		 * @return The updated transform values or an error
		 */
		static TResult<FComponentTransformResult> SetComponentTransform(const FComponentTransformParams& Params);

		/**
		 * Delete a blueprint asset.
		 *
		 * Handles the complete blueprint deletion workflow including asset loading,
		 * deletion, and registry notifications.
		 *
		 * @param Params Blueprint deletion parameters
		 * @return Deletion result with deleted path or an error
		 */
		static TResult<FDeleteBlueprintResult> DeleteBlueprint(const FDeleteBlueprintParams& Params);

		// ============ Blueprint-Level Operations ============

		/**
		 * Set a property on a blueprint's default object.
		 *
		 * Modifies a property on the blueprint's class default object,
		 * affecting all instances of this blueprint. Blueprint is marked as modified.
		 *
		 * @param BlueprintName Name of the blueprint to modify
		 * @param PropertyParams Property to set and its value
		 * @return Success or an error
		 */
		static FVoidResult SetBlueprintProperty(const FString& BlueprintName, const FPropertyParams& PropertyParams);

		/**
		 * Set multiple properties on a Pawn blueprint.
		 *
		 * Convenience method for setting common pawn-specific properties like
		 * AutoPossessPlayer and controller rotation flags. Blueprint is marked as modified.
		 *
		 * @param BlueprintName Name of the pawn blueprint to modify
		 * @param PropertyParams JSON object containing pawn property mappings
		 * @return Success or an error
		 */
		static FVoidResult
		SetPawnProperties(const FString& BlueprintName, const TSharedPtr<FJsonObject>& PropertyParams);

	private:
		// ============ Component Node Lookup ============

		/**
		 * Find a component node in a blueprint's construction script.
		 *
		 * @param Blueprint Blueprint to search
		 * @param ComponentName Name of the component to find
		 * @return The node pointer or nullptr if not found
		 */
		static USCS_Node* FindComponentNode(const UBlueprint* Blueprint, const FString& ComponentName);

		/**
		 * Validate that a blueprint has a valid construction script.
		 *
		 * @param Blueprint Blueprint to validate
		 * @return Empty string if valid, error message if invalid
		 */
		static FString ValidateBlueprintForComponentOps(const UBlueprint* Blueprint);

		/**
		 * Resolve a component type name to a UClass.
		 *
		 * Attempts multiple naming conventions:
		 * - Exact name
		 * - With "Component" suffix
		 * - With "U" prefix
		 * - With both "U" prefix and "Component" suffix
		 *
		 * @param ComponentType The component type to resolve
		 * @return Valid component class or nullptr if not found
		 */
		static UClass* ResolveComponentClass(const FString& ComponentType);
	};
}
