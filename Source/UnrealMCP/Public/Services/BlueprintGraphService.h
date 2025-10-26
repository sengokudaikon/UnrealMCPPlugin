#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"

class UBlueprint;
class UEdGraph;
class UEdGraphNode;
class UK2Node_Event;
class UK2Node_CallFunction;
class UK2Node_VariableGet;
class UK2Node_InputAction;
class UK2Node_Self;

namespace UnrealMCP {
	/**
	 * Service for Blueprint Graph operations
	 * Handles node creation, connection, and manipulation in blueprint graphs
	 * All methods are static - this is a stateless utility service
	 */
	class UNREALMCP_API FBlueprintGraphService {
	public:
		/**
		 * Connect two blueprint nodes via their pins
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param SourceNodeId GUID of the source node
		 * @param TargetNodeId GUID of the target node
		 * @param SourcePinName Name of the source pin
		 * @param TargetPinName Name of the target pin
		 * @return Success if nodes were connected, Failure with error message
		 */
		static auto ConnectNodes(
			const FString& BlueprintName,
			const FString& SourceNodeId,
			const FString& TargetNodeId,
			const FString& SourcePinName,
			const FString& TargetPinName
		) -> FVoidResult;

		/**
		 * Add an event node to a blueprint graph
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param EventName Name of the event (e.g., "ReceiveBeginPlay")
		 * @param NodePosition Position for the node in the graph
		 * @return Success with the created node, Failure with error message
		 */
		static auto AddEventNode(
			const FString& BlueprintName,
			const FString& EventName,
			const FVector2D& NodePosition
		) -> TResult<UK2Node_Event*>;

		/**
		 * Add a function call node to a blueprint graph
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param FunctionName Name of the function to call
		 * @param TargetClass Optional target class name for the function
		 * @param NodePosition Position for the node in the graph
		 * @param Parameters Optional map of parameter names to values
		 * @return Success with the created node, Failure with error message
		 */
		static auto AddFunctionCallNode(
			const FString& BlueprintName,
			const FString& FunctionName,
			const TOptional<FString>& TargetClass,
			const FVector2D& NodePosition,
			const TSharedPtr<FJsonObject>& Parameters
		) -> TResult<UK2Node_CallFunction*>;

		/**
		 * Add a variable get node for a component to a blueprint graph
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param ComponentName Name of the component to reference
		 * @param NodePosition Position for the node in the graph
		 * @return Success with the created node, Failure with error message
		 */
		static auto AddComponentReferenceNode(
			const FString& BlueprintName,
			const FString& ComponentName,
			const FVector2D& NodePosition
		) -> TResult<UK2Node_VariableGet*>;

		/**
		 * Add a self reference node to a blueprint graph
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param NodePosition Position for the node in the graph
		 * @return Success with the created node, Failure with error message
		 */
		static auto AddSelfReferenceNode(
			const FString& BlueprintName,
			const FVector2D& NodePosition
		) -> TResult<UK2Node_Self*>;

		/**
		 * Add an input action node to a blueprint graph
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param ActionName Name of the input action
		 * @param NodePosition Position for the node in the graph
		 * @return Success with the created node, Failure with error message
		 */
		static auto AddInputActionNode(
			const FString& BlueprintName,
			const FString& ActionName,
			const FVector2D& NodePosition
		) -> TResult<UK2Node_InputAction*>;

		/**
		 * Find nodes in a blueprint graph by type and criteria
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param NodeType Type of nodes to find (e.g., "Event")
		 * @param EventName Optional event name for event node searches
		 * @param OutNodeGuids Array to populate with found node GUIDs
		 * @return Success if search completed, Failure with error message
		 */
		static auto FindNodes(
			const FString& BlueprintName,
			const FString& NodeType,
			const TOptional<FString>& EventName,
			TArray<FString>& OutNodeGuids
		) -> FVoidResult;

		/**
		 * Add a variable to a blueprint
		 *
		 * @param BlueprintName Name of the blueprint
		 * @param VariableName Name of the variable
		 * @param VariableType Type of the variable (Boolean, Integer, Float, String, Vector)
		 * @param bIsExposed Whether the variable is exposed to the editor
		 * @return Success if variable was added, Failure with error message
		 */
		static auto AddVariable(
			const FString& BlueprintName,
			const FString& VariableName,
			const FString& VariableType,
			bool bIsExposed
		) -> FVoidResult;

	private:
		/**
		 * Helper to find a blueprint by name
		 */
		static UBlueprint* FindBlueprint(const FString& BlueprintName, FString& OutError);

		/**
		 * Helper to get or create the event graph for a blueprint
		 */
		static UEdGraph* GetEventGraph(UBlueprint* Blueprint, FString& OutError);

		/**
		 * Helper to find a node by GUID in a graph
		 */
		static UEdGraphNode* FindNodeByGuid(UEdGraph* Graph, const FString& NodeGuid);

		/**
		 * Helper to set function node parameters from JSON
		 */
		static auto SetFunctionParameters(
			UK2Node_CallFunction* FunctionNode,
			UEdGraph* EventGraph,
			const TSharedPtr<FJsonObject>& Parameters
		) -> FVoidResult;
	};
}
