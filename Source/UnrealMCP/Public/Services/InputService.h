#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"
#include "Core/MCPTypes.h"

class UInputAction;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

namespace UnrealMCP {
	/**
	 * Service for Enhanced Input System operations
	 * Handles creation and management of input actions, mapping contexts, and key mappings
	 * All methods are static - this is a stateless utility service
	 */
	class UNREALMCP_API FInputService {
	public:
		/**
		 * Create a new input action asset
		 *
		 * Validates parameters, creates a package, and saves the input action asset to disk.
		 * The input action name is prefixed with "IA_" following Unreal Engine naming conventions.
		 *
		 * Params.Name - The input action name (will be prefixed with "IA_")
		 * Params.ValueType - The value type (Boolean, Axis1D, Axis2D, Axis3D)
		 * Params.Path - The content browser path where the asset will be created
		 *
		 * Returns Success with the created UInputAction*, or Failure with an error message
		 */
		static auto CreateInputAction(const FInputActionParams& Params) -> TResult<UInputAction*>;

		/**
		 * Create a new input mapping context asset
		 *
		 * Validates parameters, creates a package, and saves the mapping context asset to disk.
		 * The mapping context name is prefixed with "IMC_" following Unreal Engine naming conventions.
		 *
		 * Params.Name - The mapping context name (will be prefixed with "IMC_")
		 * Params.Path - The content browser path where the asset will be created
		 *
		 * Returns Success with the created UInputMappingContext*, or Failure with an error message
		 */
		static auto CreateInputMappingContext(const FInputMappingContextParams& Params) -> TResult<UInputMappingContext*>;

		/**
		 * Add a key mapping to an input mapping context
		 *
		 * Loads both the mapping context and input action, creates a key mapping,
		 * marks the context as modified, and saves to disk.
		 *
		 * Params.ContextPath - Full asset path to the input mapping context
		 * Params.ActionPath - Full asset path to the input action
		 * Params.Key - The key name (e.g., "Space", "E", "LeftMouseButton")
		 *
		 * Returns Success if the mapping was added, or Failure with an error message
		 */
		static auto AddMappingToContext(const FAddMappingParams& Params) -> FVoidResult;

		/**
		 * Remove a key mapping from an input mapping context
		 *
		 * Loads both the mapping context and input action, removes all mappings
		 * for the action, marks the context as modified, and saves to disk.
		 *
		 * Params.ContextPath - Full asset path to the input mapping context
		 * Params.ActionPath - Full asset path to the input action
		 *
		 * Returns Success if the mapping was removed, or Failure with an error message
		 */
		static auto RemoveMappingFromContext(const FAddMappingParams& Params) -> FVoidResult;

		/**
		 * Apply a mapping context to the local player's input subsystem at runtime
		 *
		 * Loads the mapping context and adds it to the Enhanced Input subsystem
		 * for the first player controller with the specified priority.
		 *
		 * Params.ContextPath - Full asset path to the input mapping context
		 * Params.Priority - Priority for the mapping context (higher priority = evaluated first)
		 *
		 * Returns Success if the context was applied, or Failure with an error message
		 */
		static auto ApplyMappingContext(const FApplyMappingContextParams& Params) -> FVoidResult;

		/**
		 * Remove a mapping context from the local player's input subsystem at runtime
		 *
		 * Loads the mapping context and removes it from the Enhanced Input subsystem
		 * for the first player controller.
		 *
		 * Params.ContextPath - Full asset path to the input mapping context
		 *
		 * Returns Success if the context was removed, or Failure with an error message
		 */
		static auto RemoveMappingContext(const FRemoveMappingContextParams& Params) -> FVoidResult;

		/**
		 * Clear all mapping contexts from the local player's input subsystem at runtime
		 *
		 * Removes all mapping contexts from the Enhanced Input subsystem
		 * for the first player controller.
		 *
		 * Returns Success if contexts were cleared, or Failure with an error message
		 */
		static auto ClearAllMappingContexts() -> FVoidResult;

		/**
		 * Create a legacy input action mapping
		 *
		 * Adds an action mapping to the default input settings using the legacy input system.
		 * This modifies the project's input settings and saves the configuration.
		 *
		 * Params.ActionName - The action name for the mapping
		 * Params.Key - The key to bind to the action
		 * Params.bShift - Whether Shift modifier is required
		 * Params.bCtrl - Whether Ctrl modifier is required
		 * Params.bAlt - Whether Alt modifier is required
		 * Params.bCmd - Whether Cmd modifier is required
		 *
		 * Returns Success if the mapping was created, or Failure with an error message
		 */
		static auto CreateLegacyInputMapping(const FLegacyInputMappingParams& Params) -> FVoidResult;

	private:
		/**
		 * Helper to parse value type string to EInputActionValueType enum
		 * Returns Boolean as default if the string is not recognized
		 */
		static auto ParseValueType(const FString& ValueTypeStr) -> uint8;

		/**
		 * Helper to validate and load an input action from a path
		 * Returns the loaded asset, or nullptr with the provided error reference set
		 */
		static auto LoadInputAction(const FString& AssetPath, FString& OutError) -> UInputAction*;

		/**
		 * Helper to validate and load an input mapping context from a path
		 * Returns the loaded asset, or nullptr with the provided error reference set
		 */
		static auto LoadInputMappingContext(const FString& AssetPath, FString& OutError) -> UInputMappingContext*;

		/**
		 * Helper to save a package to disk with standard settings
		 * Returns true if save succeeded, false otherwise
		 */
		static auto SavePackage(UPackage* Package, UObject* Asset, const FString& PackagePath) -> bool;

		/**
		 * Helper to get the Enhanced Input subsystem from the first player controller
		 * Returns the subsystem, or nullptr with the provided error reference set
		 */
		static UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem(FString& OutError);
	};
}
