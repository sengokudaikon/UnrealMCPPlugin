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
		static TResult<FBlueprintSpawnParams> FromJson(const TSharedPtr<FJsonObject>& Json);
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
		static TResult<FBlueprintCreationParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

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
		static TResult<FComponentParams> FromJson(const TSharedPtr<FJsonObject>& Json);
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
		static TResult<FPropertyParams> FromJson(const TSharedPtr<FJsonObject>& Json, const FString& TargetFieldName = TEXT("blueprint_name"));
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
		static TResult<FPhysicsParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for creating a UMG widget blueprint
	 */
	struct FWidgetCreationParams
	{
		FString Name;
		FString ParentClass = TEXT("UserWidget");
		FString PackagePath = TEXT("/Game/UI");

		/** Parse from JSON parameters */
		static TResult<FWidgetCreationParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for adding a text block to a widget
	 */
	struct FTextBlockParams
	{
		FString WidgetName;
		FString TextBlockName;
		FString Text = TEXT("");
		TOptional<FVector2D> Position;
		TOptional<FVector2D> Size;
		int32 FontSize = 12;
		TOptional<FLinearColor> Color;

		/** Parse from JSON parameters */
		static TResult<FTextBlockParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for adding a button to a widget
	 */
	struct FButtonParams
	{
		FString WidgetName;
		FString ButtonName;
		FString Text = TEXT("");
		TOptional<FVector2D> Position;
		TOptional<FVector2D> Size;
		int32 FontSize = 12;
		TOptional<FLinearColor> TextColor;
		TOptional<FLinearColor> BackgroundColor;

		/** Parse from JSON parameters */
		static TResult<FButtonParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for binding a widget event
	 */
	struct FWidgetEventBindingParams
	{
		FString WidgetName;
		FString WidgetComponentName;
		FString EventName;
		FString FunctionName;

		/** Parse from JSON parameters */
		static TResult<FWidgetEventBindingParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for setting a text block binding
	 */
	struct FTextBlockBindingParams
	{
		FString WidgetName;
		FString TextBlockName;
		FString BindingProperty;
		FString BindingType = TEXT("Text");

		/** Parse from JSON parameters */
		static TResult<FTextBlockBindingParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for adding a widget to viewport
	 */
	struct FAddWidgetToViewportParams
	{
		FString WidgetName;
		int32 ZOrder = 0;

		/** Parse from JSON parameters */
		static TResult<FAddWidgetToViewportParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for creating an input action
	 */
	struct FInputActionParams
	{
		FString Name;
		FString ValueType = TEXT("Boolean");
		FString Path = TEXT("/Game/Input");

		/** Parse from JSON parameters */
		static TResult<FInputActionParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for creating an input mapping context
	 */
	struct FInputMappingContextParams
	{
		FString Name;
		FString Path = TEXT("/Game/Input");

		/** Parse from JSON parameters */
		static TResult<FInputMappingContextParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for adding a mapping to a context
	 */
	struct FAddMappingParams
	{
		FString ContextPath;
		FString ActionPath;
		FString Key;

		/** Parse from JSON parameters */
		static TResult<FAddMappingParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for applying a mapping context at runtime
	 */
	struct FApplyMappingContextParams
	{
		FString ContextPath;
		int32 Priority = 0;

		/** Parse from JSON parameters */
		static TResult<FApplyMappingContextParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for removing a mapping context at runtime
	 */
	struct FRemoveMappingContextParams
	{
		FString ContextPath;

		/** Parse from JSON parameters */
		static TResult<FRemoveMappingContextParams> FromJson(const TSharedPtr<FJsonObject>& Json);
	};

	/**
	 * Parameters for creating a legacy input action mapping
	 */
	struct FLegacyInputMappingParams
	{
		FString ActionName;
		FString Key;
		bool bShift = false;
		bool bCtrl = false;
		bool bAlt = false;
		bool bCmd = false;

		/** Parse from JSON parameters */
		static TResult<FLegacyInputMappingParams> FromJson(const TSharedPtr<FJsonObject>& Json);
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
		static TResult<FStaticMeshParams> FromJson(const TSharedPtr<FJsonObject>& Json);
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
		static TResult<FComponentTransformParams> FromJson(const TSharedPtr<FJsonObject>& Json);
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
		static TResult<FComponentHierarchyParams> FromJson(const TSharedPtr<FJsonObject>& Json);
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
	 * Parameters for deleting a blueprint
	 */
	struct FDeleteBlueprintParams
	{
		FString BlueprintName;

		/** Parse from JSON parameters */
		static TResult<FDeleteBlueprintParams> FromJson(const TSharedPtr<FJsonObject>& Json);
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
