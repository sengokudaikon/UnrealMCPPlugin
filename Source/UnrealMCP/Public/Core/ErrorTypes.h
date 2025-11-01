#pragma once

#include "CoreMinimal.h"

namespace UnrealMCP {

	/**
	 * Enumeration of different error types that can occur in MCP operations
	 * This provides type-safe error handling instead of matching error message strings
	 */
	UENUM()
	enum class EErrorCode : uint8 {
		// General errors
		Unknown = 0,
		InvalidInput,
		OperationFailed,

		// Actor-related errors
		ActorNotFound,
		InvalidActorClass,
		FailedToSpawnActor,
		FailedToDestroyActor,
		PropertyNotFound,
		InvalidPropertyValue,

		// Blueprint-related errors
		BlueprintNotFound,
		FailedToCompileBlueprint,
		InvalidParentClass,
		ComponentNotFound,
		FunctionNotFound,
		VariableNotFound,
		NodeNotFound,
		InvalidBlueprintStructure,
		BlueprintNotReady,
		BlueprintNotCompiled,
		InvalidBlueprintType,
		BlueprintInvalid,
		PropertySetFailed,
		NodeCreationFailed,
		NodeConnectionFailed,
		BlueprintHasNoConstructionScript,

		// Asset-related errors
		AssetNotFound,
		FailedToCreateAsset,
		FailedToSaveAsset,
		FailedToDeleteAsset,

		// Editor/World-related errors
		WorldNotFound,
		EditorSubsystemNotFound,
		InvalidOperationInCurrentContext,

		// Component-related errors
		InvalidComponentType,
		FailedToCreateComponent,
		FailedToAttachComponent,

		// Input-related errors
		InputActionNotFound,
		InputMappingNotFound,
		InvalidInputConfiguration,

		// UI/Widget-related errors
		WidgetNotFound,
		InvalidWidgetType,
		FailedToCreateWidget,
		FailedToBindWidgetEvent
	};

	/**
	 * Structured error type that contains both the error code and contextual information
	 */
	struct FError {
		EErrorCode Code = EErrorCode::Unknown;
		FString Context;
		FString Details;

		FError() noexcept = default;

		explicit FError(const EErrorCode InCode) noexcept : Code(InCode) {}

		FError(const EErrorCode InCode, const FString& InContext)
			: Code(InCode), Context(InContext) {}

		FError(const EErrorCode InCode, const FString& InContext, const FString& InDetails)
			: Code(InCode), Context(InContext), Details(InDetails) {}

		/** Check if this error is empty (no error) */
		[[nodiscard]] bool IsEmpty() const {
			return Code == EErrorCode::Unknown && Context.IsEmpty();
		}

		/**
		 * Get a human-readable error message (for logging/debugging purposes only)
		 * Tests should check Error.Code, not Error.GetMessage()
		 */
		[[nodiscard]] FString GetMessage() const {
			FString Result = GetErrorCodeName(Code);
			if (!Context.IsEmpty()) {
				Result += FString::Printf(TEXT(": %s"), *Context);
			}
			if (!Details.IsEmpty()) {
				Result += FString::Printf(TEXT(" (%s)"), *Details);
			}
			return Result;
		}

		/**
		 * Convert error code to string representation
		 */
		[[nodiscard]] static FString GetErrorCodeName(const EErrorCode ErrorCode) noexcept {
			switch (ErrorCode) {
				case EErrorCode::Unknown: return TEXT("Unknown Error");
				case EErrorCode::InvalidInput: return TEXT("Invalid Input");
				case EErrorCode::OperationFailed: return TEXT("Operation Failed");

				case EErrorCode::ActorNotFound: return TEXT("Actor Not Found");
				case EErrorCode::InvalidActorClass: return TEXT("Invalid Actor Class");
				case EErrorCode::FailedToSpawnActor: return TEXT("Failed to Spawn Actor");
				case EErrorCode::FailedToDestroyActor: return TEXT("Failed to Destroy Actor");
				case EErrorCode::PropertyNotFound: return TEXT("Property Not Found");
				case EErrorCode::InvalidPropertyValue: return TEXT("Invalid Property Value");

				case EErrorCode::BlueprintNotFound: return TEXT("Blueprint Not Found");
				case EErrorCode::FailedToCompileBlueprint: return TEXT("Failed to Compile Blueprint");
				case EErrorCode::InvalidParentClass: return TEXT("Invalid Parent Class");
				case EErrorCode::ComponentNotFound: return TEXT("Component Not Found");
				case EErrorCode::FunctionNotFound: return TEXT("Function Not Found");
				case EErrorCode::VariableNotFound: return TEXT("Variable Not Found");
				case EErrorCode::NodeNotFound: return TEXT("Node Not Found");
				case EErrorCode::InvalidBlueprintStructure: return TEXT("Invalid Blueprint Structure");
				case EErrorCode::BlueprintNotReady: return TEXT("Blueprint Not Ready");
				case EErrorCode::BlueprintNotCompiled: return TEXT("Blueprint Not Compiled");
				case EErrorCode::InvalidBlueprintType: return TEXT("Invalid Blueprint Type");
				case EErrorCode::BlueprintInvalid: return TEXT("Blueprint Invalid");
				case EErrorCode::PropertySetFailed: return TEXT("Property Set Failed");
				case EErrorCode::NodeCreationFailed: return TEXT("Node Creation Failed");
				case EErrorCode::NodeConnectionFailed: return TEXT("Node Connection Failed");
				case EErrorCode::BlueprintHasNoConstructionScript: return TEXT("Blueprint Has No Construction Script");

				case EErrorCode::AssetNotFound: return TEXT("Asset Not Found");
				case EErrorCode::FailedToCreateAsset: return TEXT("Failed to Create Asset");
				case EErrorCode::FailedToSaveAsset: return TEXT("Failed to Save Asset");
				case EErrorCode::FailedToDeleteAsset: return TEXT("Failed to Delete Asset");

				case EErrorCode::WorldNotFound: return TEXT("World Not Found");
				case EErrorCode::EditorSubsystemNotFound: return TEXT("Editor Subsystem Not Found");
				case EErrorCode::InvalidOperationInCurrentContext: return TEXT("Invalid Operation in Current Context");

				case EErrorCode::InvalidComponentType: return TEXT("Invalid Component Type");
				case EErrorCode::FailedToCreateComponent: return TEXT("Failed to Create Component");
				case EErrorCode::FailedToAttachComponent: return TEXT("Failed to Attach Component");

				case EErrorCode::InputActionNotFound: return TEXT("Input Action Not Found");
				case EErrorCode::InputMappingNotFound: return TEXT("Input Mapping Not Found");
				case EErrorCode::InvalidInputConfiguration: return TEXT("Invalid Input Configuration");

				case EErrorCode::WidgetNotFound: return TEXT("Widget Not Found");
				case EErrorCode::InvalidWidgetType: return TEXT("Invalid Widget Type");
				case EErrorCode::FailedToCreateWidget: return TEXT("Failed to Create Widget");
				case EErrorCode::FailedToBindWidgetEvent: return TEXT("Failed to Bind Widget Event");

				default: return TEXT("Undefined Error");
			}
		}
	};

} // namespace UnrealMCP