#pragma once

#include "CoreMinimal.h"

/**
 * Handles spawning instances of blueprint actors in the level
 * Takes a blueprint asset path and creates an actor instance from that blueprint
 * For spawning native engine actors (StaticMeshActor, PointLight, etc.), use FSpawnActor in Commands/Editor
 */
class UNREALMCP_API FSpawnActorBlueprint
{
public:
	FSpawnActorBlueprint() = default;
	~FSpawnActorBlueprint() = default;

	/**
	 * Handle blueprint spawning command
	 * @param Params JSON parameters containing blueprint_name, actor_name, and optional location/rotation
	 * @return JSON response with spawned actor data or error message
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
