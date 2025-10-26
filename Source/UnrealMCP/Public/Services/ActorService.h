#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"

class AActor;
class UWorld;

namespace UnrealMCP {
	/**
	 * Service for actor manipulation operations in the editor
	 * Handles spawning, deleting, finding, and modifying actors
	 * All methods are static - this is a stateless utility service
	 */
	class UNREALMCP_API FActorService {
	public:
		/**
		 * Get all actors in the current level
		 *
		 * @param OutActorNames Array to populate with actor names
		 * @return Success if actors were retrieved, Failure otherwise
		 */
		static auto GetActorsInLevel(TArray<FString>& OutActorNames) -> FVoidResult;

		/**
		 * Find actors by name (partial match)
		 *
		 * @param NamePattern Pattern to match against actor names
		 * @param OutActorNames Array to populate with matching actor names
		 * @return Success if search completed, Failure otherwise
		 */
		static auto FindActorsByName(const FString& NamePattern, TArray<FString>& OutActorNames) -> FVoidResult;

		/**
		 * Spawn a new actor in the level
		 *
		 * @param ActorClass Class of actor to spawn (e.g., "StaticMeshActor", "PointLight")
		 * @param ActorName Name for the new actor
		 * @param Location Optional spawn location
		 * @param Rotation Optional spawn rotation
		 * @return Success with the spawned actor, Failure with error message
		 */
		static auto SpawnActor(
			const FString& ActorClass,
			const FString& ActorName,
			const TOptional<FVector>& Location,
			const TOptional<FRotator>& Rotation
		) -> TResult<AActor*>;

		/**
		 * Delete an actor from the level
		 *
		 * @param ActorName Name of the actor to delete
		 * @return Success if actor was deleted, Failure with error message
		 */
		static auto DeleteActor(const FString& ActorName) -> FVoidResult;

		/**
		 * Set an actor's transform
		 *
		 * Note: Transform operations work best with actors that have proper scene components
		 * (like StaticMeshActor, PointLight, etc.). Basic AActor instances may not respond
		 * to transform changes if they don't have root components. This method will automatically
		 * create a root component for actors that don't have one, but for predictable transform
		 * behavior, use actor types that inherently support transforms.
		 *
		 * @param ActorName Name of the actor
		 * @param Location Optional new location
		 * @param Rotation Optional new rotation
		 * @param Scale Optional new scale
		 * @return Success if transform was set, Failure with error message
		 */
		static auto SetActorTransform(
			const FString& ActorName,
			const TOptional<FVector>& Location,
			const TOptional<FRotator>& Rotation,
			const TOptional<FVector>& Scale
		) -> FVoidResult;

		/**
		 * Get an actor's properties
		 *
		 * @param ActorName Name of the actor
		 * @param OutProperties Map to populate with property name/value pairs
		 * @return Success if properties were retrieved, Failure with error message
		 */
		static auto GetActorProperties(const FString& ActorName, TMap<FString, FString>& OutProperties) -> FVoidResult;

		/**
		 * Set an actor's property
		 *
		 * @param ActorName Name of the actor
		 * @param PropertyName Name of the property to set
		 * @param PropertyValue Value to set (as JSON value for type flexibility)
		 * @return Success if property was set, Failure with error message
		 */
		static auto SetActorProperty(
			const FString& ActorName,
			const FString& PropertyName,
			const TSharedPtr<FJsonValue>& PropertyValue
		) -> FVoidResult;

	private:
		/**
		 * Helper to get the current editor world
		 */
		static UWorld* GetEditorWorld();

		/**
		 * Helper to find an actor by name in the current world
		 */
		static AActor* FindActorByName(const FString& ActorName);

		/**
		 * Helper to map actor class string to UClass
		 */
		static UClass* GetActorClassByName(const FString& ClassName);
	};
}
