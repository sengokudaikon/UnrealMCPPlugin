#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"
#include "Core/MCPTypes.h"

class UBlueprint;

namespace UnrealMCP {
	/**
	 * Service for Blueprint member operations (functions and variables).
	 *
	 * Handles all operations related to blueprint functions and variables including:
	 * - Function creation, deletion, and configuration
	 * - Function parameters and return types
	 * - Variable creation, deletion, renaming
	 * - Variable metadata and default values
	 *
	 * All methods are static and return TResult for type-safe error handling.
	 */
	class UNREALMCP_API FBlueprintMemberService {
	public:
		// ============ Function Operations ============

		/**
		 * Add a custom function to a blueprint.
		 *
		 * Creates a new function graph with entry and result nodes.
		 * The blueprint is marked as modified and compiled after creation.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param FunctionName Name of the function to create
		 * @return Success with function name, or error if creation fails
		 */
		static auto AddFunction(
			const FString& BlueprintName,
			const FString& FunctionName
		) -> TResult<FString>;

		/**
		 * Remove a function from a blueprint.
		 *
		 * Deletes the function graph and all associated nodes.
		 * The blueprint is marked as modified and compiled after deletion.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param FunctionName Name of the function to remove
		 * @return Success or error if removal fails
		 */
		static auto RemoveFunction(
			const FString& BlueprintName,
			const FString& FunctionName
		) -> FVoidResult;

		/**
		 * Add a parameter to a blueprint function.
		 *
		 * Adds an input parameter with specified name, type, and reference behavior.
		 * The blueprint is marked as modified and compiled after parameter addition.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param FunctionName Name of the function
		 * @param ParamName Name of the parameter to add
		 * @param ParamType Type of the parameter (bool, int, float, string, name, vector, rotator, transform)
		 * @param bIsReference Whether the parameter is passed by reference
		 * @return Success or error if addition fails
		 */
		static auto AddFunctionParameter(
			const FString& BlueprintName,
			const FString& FunctionName,
			const FString& ParamName,
			const FString& ParamType,
			bool bIsReference
		) -> FVoidResult;

		/**
		 * Set the return type of a blueprint function.
		 *
		 * Configures the output pin on the function's result node.
		 * Creates a result node if one doesn't exist.
		 * The blueprint is marked as modified and compiled after configuration.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param FunctionName Name of the function
		 * @param ReturnType Type of the return value (bool, int, float, string, name, vector, rotator, transform)
		 * @return Success or error if configuration fails
		 */
		static auto SetFunctionReturnType(
			const FString& BlueprintName,
			const FString& FunctionName,
			const FString& ReturnType
		) -> FVoidResult;

		/**
		 * Set metadata properties on a blueprint function.
		 *
		 * Configures category, tooltip, and pure flag for better organization and documentation.
		 * The blueprint is marked as modified and compiled after metadata update.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param FunctionName Name of the function
		 * @param Category Optional function category for organization
		 * @param Tooltip Optional function description/tooltip
		 * @param bPure Optional flag for pure functions (no side effects)
		 * @return Success or error if configuration fails
		 */
		static auto SetFunctionMetadata(
			const FString& BlueprintName,
			const FString& FunctionName,
			const TOptional<FString>& Category,
			const TOptional<FString>& Tooltip,
			const TOptional<bool>& bPure
		) -> FVoidResult;

		/**
		 * Get all functions from a blueprint.
		 *
		 * Retrieves detailed information about all functions in a blueprint including
		 * parameters, return values, metadata, and node counts.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @return Result with function information or error
		 */
		static auto GetFunctions(const FString& BlueprintName) -> TResult<FGetBlueprintFunctionsResult>;

		// ============ Variable Operations ============

		/**
		 * Remove a variable from a blueprint.
		 *
		 * Deletes the variable and updates all references.
		 * The blueprint is marked as modified and compiled after deletion.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param VariableName Name of the variable to remove
		 * @return Success or error if removal fails
		 */
		static auto RemoveVariable(
			const FString& BlueprintName,
			const FString& VariableName
		) -> FVoidResult;

		/**
		 * Rename a variable in a blueprint.
		 *
		 * Renames the variable and updates all references throughout the blueprint.
		 * The blueprint is marked as modified and compiled after renaming.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param OldName Current name of the variable
		 * @param NewName New name for the variable
		 * @return Success or error if renaming fails
		 */
		static auto RenameVariable(
			const FString& BlueprintName,
			const FString& OldName,
			const FString& NewName
		) -> FVoidResult;

		/**
		 * Set the default value of a blueprint variable.
		 *
		 * Supports bool, int, float, string, Vector, Rotator, and other basic types.
		 * The value is passed as a JSON value for type flexibility.
		 * The blueprint is marked as modified and compiled after value update.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param VariableName Name of the variable
		 * @param Value Default value (type depends on variable type)
		 * @return Success or error if value setting fails
		 */
		static auto SetVariableDefaultValue(
			const FString& BlueprintName,
			const FString& VariableName,
			const TSharedPtr<FJsonValue>& Value
		) -> FVoidResult;

		/**
		 * Set metadata properties on a blueprint variable.
		 *
		 * Configures tooltip, category, visibility, and editability flags.
		 * The blueprint is marked as modified and compiled after metadata update.
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param VariableName Name of the variable
		 * @param Tooltip Optional tooltip text
		 * @param Category Optional category name
		 * @param bExposeOnSpawn Optional flag to expose variable on spawn
		 * @param bInstanceEditable Optional flag to make variable editable per-instance
		 * @param bBlueprintReadOnly Optional flag to make variable read-only in BP
		 * @return Success or error if configuration fails
		 */
		static auto SetVariableMetadata(
			const FString& BlueprintName,
			const FString& VariableName,
			const TOptional<FString>& Tooltip,
			const TOptional<FString>& Category,
			const TOptional<bool>& bExposeOnSpawn,
			const TOptional<bool>& bInstanceEditable,
			const TOptional<bool>& bBlueprintReadOnly
		) -> FVoidResult;
	};

}
