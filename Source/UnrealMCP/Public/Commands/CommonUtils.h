#pragma once

#include "CoreMinimal.h"

// Forward declarations
class AActor;
class UBlueprint;
class UEdGraph;
class UEdGraphNode;
class UEdGraphPin;
class UK2Node_Event;
class UK2Node_CallFunction;
class UK2Node_VariableGet;
class UK2Node_VariableSet;
class UK2Node_InputAction;
class UK2Node_Self;
class UFunction;

/**
 * Common utilities for UnrealMCP commands
 */
class UNREALMCP_API FCommonUtils {
public:
	// JSON utilities
	static auto CreateErrorResponse(const FString& Message) -> TSharedPtr<FJsonObject>;

	static auto CreateSuccessResponse(const TSharedPtr<FJsonObject>& Data = nullptr) -> TSharedPtr<FJsonObject>;

	static void GetIntArrayFromJson(
		const TSharedPtr<FJsonObject>& JsonObject,
		const FString& FieldName,
		TArray<int32>& OutArray
	);

	static void GetFloatArrayFromJson(
		const TSharedPtr<FJsonObject>& JsonObject,
		const FString& FieldName,
		TArray<float>& OutArray
	);

	static auto GetVector2DFromJson(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) -> FVector2D;

	static auto GetVectorFromJson(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) -> FVector;

	static auto GetRotatorFromJson(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) -> FRotator;

	static auto ActorToJson(AActor* Actor) -> TSharedPtr<FJsonValue>;

	static auto ActorToJsonObject(AActor* Actor, bool bDetailed = false) -> TSharedPtr<FJsonObject>;

	static auto FindBlueprint(const FString& BlueprintName) -> UBlueprint*;

	static auto FindBlueprintByName(const FString& BlueprintName) -> UBlueprint*;

	static auto FindOrCreateEventGraph(UBlueprint* Blueprint) -> UEdGraph*;

	static auto CreateEventNode(UEdGraph* Graph, const FString& EventName, const FVector2D& Position) -> UK2Node_Event*;

	static auto CreateFunctionCallNode(
		UEdGraph* Graph,
		UFunction* Function,
		const FVector2D& Position
	) -> UK2Node_CallFunction*;

	static auto CreateVariableGetNode(
		UEdGraph* Graph,
		UBlueprint* Blueprint,
		const FString& VariableName,
		const FVector2D& Position
	) -> UK2Node_VariableGet*;

	static auto CreateVariableSetNode(
		UEdGraph* Graph,
		UBlueprint* Blueprint,
		const FString& VariableName,
		const FVector2D& Position
	) -> UK2Node_VariableSet*;

	static auto CreateInputActionNode(
		UEdGraph* Graph,
		const FString& ActionName,
		const FVector2D& Position
	) -> UK2Node_InputAction*;

	static auto CreateSelfReferenceNode(UEdGraph* Graph, const FVector2D& Position) -> UK2Node_Self*;

	static bool ConnectGraphNodes(
		UEdGraph* Graph,
		UEdGraphNode* SourceNode,
		const FString& SourcePinName,
		UEdGraphNode* TargetNode,
		const FString& TargetPinName
	);

	static auto FindPin(
		UEdGraphNode* Node,
		const FString& PinName,
		EEdGraphPinDirection Direction = EGPD_MAX
	) -> UEdGraphPin*;

	static auto FindExistingEventNode(UEdGraph* Graph, const FString& EventName) -> UK2Node_Event*;

	// Property utilities
	static bool SetObjectProperty(
		UObject* Object,
		const FString& PropertyName,
		const TSharedPtr<FJsonValue>& Value,
		FString& OutErrorMessage
	);
};
