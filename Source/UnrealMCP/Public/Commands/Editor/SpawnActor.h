#pragma once

#include "CoreMinimal.h"

/**
 * Handles spawning native actors in the editor level
 * Spawns engine actors like StaticMeshActor, PointLight, etc.
 * For blueprint actors, use FSpawnActorBlueprint in Commands/Blueprint
 */
class UNREALMCP_API FSpawnActor {
public:
	FSpawnActor() = default;
	~FSpawnActor() = default;

	/**
	 * Processes the provided JSON parameters to spawn an actor and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (actor_class, actor_name, location, rotation)
	 * @return A JSON object containing the spawned actor details or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
