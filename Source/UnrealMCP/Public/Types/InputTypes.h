#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Core/Result.h"

namespace UnrealMCP
{
	/**
	 * Parameters for creating an input action
	 */
	struct FInputActionParams
	{
		FString Name;
		FString ValueType = TEXT("Boolean");
		FString Path = TEXT("/Game/Input");

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FInputActionParams>;
	};

	/**
	 * Parameters for creating an input mapping context
	 */
	struct FInputMappingContextParams
	{
		FString Name;
		FString Path = TEXT("/Game/Input");

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FInputMappingContextParams>;
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
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FAddMappingParams>;
	};

	/**
	 * Parameters for applying a mapping context at runtime
	 */
	struct FApplyMappingContextParams
	{
		FString ContextPath;
		int32 Priority = 0;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FApplyMappingContextParams>;
	};

	/**
	 * Parameters for removing a mapping context at runtime
	 */
	struct FRemoveMappingContextParams
	{
		FString ContextPath;

		/** Parse from JSON parameters */
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FRemoveMappingContextParams>;
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
		static auto FromJson(const TSharedPtr<FJsonObject>& Json) -> TResult<FLegacyInputMappingParams>;
	};

	/**
	 * Result structure for input action creation
	 */
	struct FCreateInputActionResult
	{
		FString Name;
		FString ValueType;
		FString AssetPath;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};
}