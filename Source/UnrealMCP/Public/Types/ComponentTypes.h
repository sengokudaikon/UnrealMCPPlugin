#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Core/Result.h"

namespace UnrealMCP
{
	/**
	 * Parameters for adding a component to a blueprint
	 */
	struct FComponentParams
	{
		FString BlueprintName;
		FString ComponentType;
		FString ComponentName;
		TOptional<FString> MeshType;
		TOptional<FVector> Location;
		TOptional<FRotator> Rotation;
		TOptional<FVector> Scale;
		TSharedPtr<FJsonObject> Properties;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentParams>;
	};

	/**
	 * Parameters for setting a property
	 */
	struct FPropertyParams
	{
		FString TargetName;
		FString PropertyName;
		TSharedPtr<FJsonValue> PropertyValue;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json, const FString& TargetFieldName = TEXT("blueprint_name")) -> TResult<FPropertyParams>;
	};

	/**
	 * Parameters for physics properties
	 */
	struct FPhysicsParams
	{
		FString BlueprintName;
		FString ComponentName;
		bool bSimulatePhysics = true;
		float Mass = 1.0f;
		float LinearDamping = 0.01f;
		float AngularDamping = 0.0f;
		bool bEnableGravity = true;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FPhysicsParams>;
	};

	/**
	 * Parameters for setting static mesh properties on a component
	 */
	struct FStaticMeshParams
	{
		FString BlueprintName;
		FString ComponentName;
		FString StaticMesh;
		TOptional<FString> Material;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FStaticMeshParams>;
	};

	/**
	 * Parameters for setting transform properties on a component
	 */
	struct FComponentTransformParams
	{
		FString BlueprintName;
		FString ComponentName;
		TOptional<FVector> Location;
		TOptional<FRotator> Rotation;
		TOptional<FVector> Scale;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentTransformParams>;
	};

	/**
	 * Result structure for component transform operations
	 */
	struct FComponentTransformResult
	{
		FVector Location;
		FRotator Rotation;
		FVector Scale;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Parameters for getting component hierarchy
	 */
	struct FComponentHierarchyParams
	{
		FString BlueprintName;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentHierarchyParams>;
	};

	/**
	 * Result structure for component hierarchy operations
	 */
	struct FComponentHierarchyResult
	{
		TArray<TSharedPtr<FJsonValue>> Hierarchy;
		int32 RootCount;
		int32 TotalComponents;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Parameters for getting component properties
	 */
	struct FComponentPropertiesParams
	{
		FString BlueprintName;
		FString ComponentName;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FComponentPropertiesParams>;
	};

	/**
	 * Result structure for component properties operations
	 */
	struct FComponentPropertiesResult
	{
		TSharedPtr<FJsonObject> Properties;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Parameters for removing a component from a blueprint
	 */
	struct FRemoveComponentParams
	{
		FString BlueprintName;
		FString ComponentName;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FRemoveComponentParams>;
	};

	/**
	 * Result structure for component removal operations
	 */
	struct FRemoveComponentResult
	{
		FString BlueprintName;
		FString ComponentName;
		FString Message;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Parameters for renaming a component in a blueprint
	 */
	struct FRenameComponentParams
	{
		FString BlueprintName;
		FString OldName;
		FString NewName;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FRenameComponentParams>;
	};

	/**
	 * Result structure for component rename operations
	 */
	struct FRenameComponentResult
	{
		FString BlueprintName;
		FString OldName;
		FString NewName;
		FString Message;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};
}