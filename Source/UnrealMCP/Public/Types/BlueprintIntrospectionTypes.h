#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Core/Result.h"

namespace UnrealMCP
{
	/**
	 * Information about a blueprint function parameter
	 */
	struct FBlueprintFunctionParam
	{
		FString Name;
		FString Type;
		TOptional<FString> SubType;
		bool bIsArray;
		bool bIsReference;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Information about a blueprint function
	 */
	struct FBlueprintFunctionInfo
	{
		FString Name;
		TArray<FBlueprintFunctionParam> Parameters;
		TArray<FBlueprintFunctionParam> Returns;
		FString Category;
		FString Tooltip;
		FString Keywords;
		bool bIsPure;
		int32 NodeCount;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Result structure for getting blueprint functions
	 */
	struct FGetBlueprintFunctionsResult
	{
		TArray<FBlueprintFunctionInfo> Functions;
		int32 Count;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Information about a blueprint variable
	 */
	struct FBlueprintVariableInfo
	{
		FString Name;
		FString Type;
		FString Category;
		FString Tooltip;
		bool bIsArray;
		bool bIsReference;
		bool bInstanceEditable;
		bool bBlueprintReadOnly;
		bool bExposeOnSpawn;
		FString DefaultValue;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};

	/**
	 * Result structure for getting blueprint variables
	 */
	struct FGetBlueprintVariablesResult
	{
		TArray<FBlueprintVariableInfo> Variables;
		int32 Count;

		/** Convert to JSON object */
		TSharedPtr<FJsonObject> ToJson() const;
	};
}