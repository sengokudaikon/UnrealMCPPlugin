#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Core/Result.h"

namespace UnrealMCP
{
	/**
	 * Parameters for spawning a blueprint actor
	 */
	struct FBlueprintSpawnParams
	{
		FString BlueprintName;
		FString ActorName;
		TOptional<FVector> Location;
		TOptional<FRotator> Rotation;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FBlueprintSpawnParams>;
	};

	/**
	 * Parameters for creating a blueprint
	 */
	struct FBlueprintCreationParams
	{
		FString Name;
		FString ParentClass;
		FString PackagePath = TEXT("/Game/Blueprints/");

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FBlueprintCreationParams>;
	};

	/**
	 * Parameters for deleting a blueprint
	 */
	struct FDeleteBlueprintParams
	{
		FString BlueprintName;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FDeleteBlueprintParams>;
	};

	/**
	 * Result structure for blueprint deletion operations
	 */
	struct FDeleteBlueprintResult
	{
		FString DeletedPath;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};
}