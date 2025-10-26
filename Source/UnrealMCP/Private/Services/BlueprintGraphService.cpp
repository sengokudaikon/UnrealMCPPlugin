#include "Services/BlueprintGraphService.h"
#include "Commands/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "K2Node_InputAction.h"
#include "K2Node_Self.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBlueprintGraphService, Log, All);

namespace UnrealMCP {

	auto FBlueprintGraphService::ConnectNodes(
		const FString& BlueprintName,
		const FString& SourceNodeId,
		const FString& TargetNodeId,
		const FString& SourcePinName,
		const FString& TargetPinName
	) -> FVoidResult {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (SourceNodeId.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Source node ID cannot be empty"));
		}
		if (TargetNodeId.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Target node ID cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return FVoidResult::Failure(Error);
		}

		UEdGraph* EventGraph = GetEventGraph(Blueprint, Error);
		if (!EventGraph) {
			return FVoidResult::Failure(Error);
		}

		UEdGraphNode* SourceNode = FindNodeByGuid(EventGraph, SourceNodeId);
		UEdGraphNode* TargetNode = FindNodeByGuid(EventGraph, TargetNodeId);

		if (!SourceNode || !TargetNode) {
			return FVoidResult::Failure(TEXT("Source or target node not found"));
		}

		if (FCommonUtils::ConnectGraphNodes(EventGraph, SourceNode, SourcePinName, TargetNode, TargetPinName)) {
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
			return FVoidResult::Success();
		}

		return FVoidResult::Failure(TEXT("Failed to connect nodes"));
	}

	auto FBlueprintGraphService::AddEventNode(
		const FString& BlueprintName,
		const FString& EventName,
		const FVector2D& NodePosition
	) -> TResult<UK2Node_Event*> {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return TResult<UK2Node_Event*>::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (EventName.IsEmpty()) {
			return TResult<UK2Node_Event*>::Failure(TEXT("Event name cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return TResult<UK2Node_Event*>::Failure(Error);
		}

		UEdGraph* EventGraph = GetEventGraph(Blueprint, Error);
		if (!EventGraph) {
			return TResult<UK2Node_Event*>::Failure(Error);
		}

		UK2Node_Event* EventNode = FCommonUtils::CreateEventNode(EventGraph, EventName, NodePosition);
		if (!EventNode) {
			return TResult<UK2Node_Event*>::Failure(TEXT("Failed to create event node"));
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return TResult<UK2Node_Event*>::Success(EventNode);
	}

	auto FBlueprintGraphService::AddFunctionCallNode(
		const FString& BlueprintName,
		const FString& FunctionName,
		const TOptional<FString>& TargetClass,
		const FVector2D& NodePosition,
		const TSharedPtr<FJsonObject>& Parameters
	) -> TResult<UK2Node_CallFunction*> {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return TResult<UK2Node_CallFunction*>::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (FunctionName.IsEmpty()) {
			return TResult<UK2Node_CallFunction*>::Failure(TEXT("Function name cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return TResult<UK2Node_CallFunction*>::Failure(Error);
		}

		UEdGraph* EventGraph = GetEventGraph(Blueprint, Error);
		if (!EventGraph) {
			return TResult<UK2Node_CallFunction*>::Failure(Error);
		}

		// Find the function
		UFunction* Function = nullptr;
		UK2Node_CallFunction* FunctionNode = nullptr;

		// Check if we have a target class specified
		if (TargetClass.IsSet() && !TargetClass.GetValue().IsEmpty()) {
			// Try to find the target class
			const UClass* ClassPtr = nullptr;

			// First try without a prefix
			ClassPtr = FindFirstObject<UClass>(*TargetClass.GetValue(), EFindFirstObjectOptions::NativeFirst);

			// If not found, try with U prefix
			if (!ClassPtr && !TargetClass.GetValue().StartsWith(TEXT("U"))) {
				const FString TargetWithPrefix = FString(TEXT("U")) + TargetClass.GetValue();
				ClassPtr = FindFirstObject<UClass>(*TargetWithPrefix, EFindFirstObjectOptions::NativeFirst);
			}

			// Special case for UGameplayStatics
			if (!ClassPtr && TargetClass.GetValue() == TEXT("UGameplayStatics")) {
				ClassPtr = FindFirstObject<UClass>(TEXT("UGameplayStatics"), EFindFirstObjectOptions::NativeFirst);
				if (!ClassPtr) {
					ClassPtr = LoadObject<UClass>(nullptr, TEXT("/Script/Engine.GameplayStatics"));
				}
			}

			if (ClassPtr) {
				Function = ClassPtr->FindFunctionByName(*FunctionName);
			}
		}

		// If we still haven't found the function, try in the blueprint's class
		if (!Function) {
			Function = Blueprint->GeneratedClass->FindFunctionByName(*FunctionName);
		}

		// Create the function call node if we found the function
		if (Function) {
			FunctionNode = FCommonUtils::CreateFunctionCallNode(EventGraph, Function, NodePosition);
		}

		if (!FunctionNode) {
			return TResult<UK2Node_CallFunction*>::Failure(FString::Printf(
				TEXT("Function not found: %s in target %s"),
				*FunctionName,
				TargetClass.IsSet() ? *TargetClass.GetValue() : TEXT("Blueprint")));
		}

		// Set parameters if provided
		if (Parameters.IsValid()) {
			const FVoidResult ParamResult = SetFunctionParameters(FunctionNode, EventGraph, Parameters);
			if (ParamResult.IsFailure()) {
				return TResult<UK2Node_CallFunction*>::Failure(ParamResult.GetError());
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return TResult<UK2Node_CallFunction*>::Success(FunctionNode);
	}

	auto FBlueprintGraphService::AddComponentReferenceNode(
		const FString& BlueprintName,
		const FString& ComponentName,
		const FVector2D& NodePosition
	) -> TResult<UK2Node_VariableGet*> {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return TResult<UK2Node_VariableGet*>::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (ComponentName.IsEmpty()) {
			return TResult<UK2Node_VariableGet*>::Failure(TEXT("Component name cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return TResult<UK2Node_VariableGet*>::Failure(Error);
		}

		UEdGraph* EventGraph = GetEventGraph(Blueprint, Error);
		if (!EventGraph) {
			return TResult<UK2Node_VariableGet*>::Failure(Error);
		}

		UK2Node_VariableGet* GetComponentNode = NewObject<UK2Node_VariableGet>(EventGraph);
		if (!GetComponentNode) {
			return TResult<UK2Node_VariableGet*>::Failure(TEXT("Failed to create get component node"));
		}

		FMemberReference& VarRef = GetComponentNode->VariableReference;
		VarRef.SetSelfMember(FName(*ComponentName));

		GetComponentNode->NodePosX = NodePosition.X;
		GetComponentNode->NodePosY = NodePosition.Y;

		EventGraph->AddNode(GetComponentNode);
		GetComponentNode->CreateNewGuid();
		GetComponentNode->PostPlacedNewNode();
		GetComponentNode->AllocateDefaultPins();
		GetComponentNode->ReconstructNode();

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return TResult<UK2Node_VariableGet*>::Success(GetComponentNode);
	}

	auto FBlueprintGraphService::AddSelfReferenceNode(
		const FString& BlueprintName,
		const FVector2D& NodePosition
	) -> TResult<UK2Node_Self*> {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return TResult<UK2Node_Self*>::Failure(TEXT("Blueprint name cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return TResult<UK2Node_Self*>::Failure(Error);
		}

		UEdGraph* EventGraph = GetEventGraph(Blueprint, Error);
		if (!EventGraph) {
			return TResult<UK2Node_Self*>::Failure(Error);
		}

		UK2Node_Self* SelfNode = FCommonUtils::CreateSelfReferenceNode(EventGraph, NodePosition);
		if (!SelfNode) {
			return TResult<UK2Node_Self*>::Failure(TEXT("Failed to create self node"));
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return TResult<UK2Node_Self*>::Success(SelfNode);
	}

	auto FBlueprintGraphService::AddInputActionNode(
		const FString& BlueprintName,
		const FString& ActionName,
		const FVector2D& NodePosition
	) -> TResult<UK2Node_InputAction*> {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return TResult<UK2Node_InputAction*>::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (ActionName.IsEmpty()) {
			return TResult<UK2Node_InputAction*>::Failure(TEXT("Action name cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return TResult<UK2Node_InputAction*>::Failure(Error);
		}

		UEdGraph* EventGraph = GetEventGraph(Blueprint, Error);
		if (!EventGraph) {
			return TResult<UK2Node_InputAction*>::Failure(Error);
		}

		UK2Node_InputAction* InputActionNode = FCommonUtils::CreateInputActionNode(EventGraph, ActionName, NodePosition);
		if (!InputActionNode) {
			return TResult<UK2Node_InputAction*>::Failure(TEXT("Failed to create input action node"));
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return TResult<UK2Node_InputAction*>::Success(InputActionNode);
	}

	auto FBlueprintGraphService::FindNodes(
		const FString& BlueprintName,
		const FString& NodeType,
		const TOptional<FString>& EventName,
		TArray<FString>& OutNodeGuids
	) -> FVoidResult {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (NodeType.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Node type cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return FVoidResult::Failure(Error);
		}

		UEdGraph* EventGraph = GetEventGraph(Blueprint, Error);
		if (!EventGraph) {
			return FVoidResult::Failure(Error);
		}

		if (NodeType == TEXT("Event")) {
			if (!EventName.IsSet()) {
				return FVoidResult::Failure(TEXT("Missing 'event_name' parameter for Event node search"));
			}

			for (UEdGraphNode* Node : EventGraph->Nodes) {
				const UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
				if (EventNode && EventNode->EventReference.GetMemberName() == FName(*EventName.GetValue())) {
					OutNodeGuids.Add(EventNode->NodeGuid.ToString());
				}
			}
		}

		return FVoidResult::Success();
	}

	auto FBlueprintGraphService::AddVariable(
		const FString& BlueprintName,
		const FString& VariableName,
		const FString& VariableType,
		const bool bIsExposed
	) -> FVoidResult {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}
		if (VariableName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Variable name cannot be empty"));
		}
		if (VariableType.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Variable type cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return FVoidResult::Failure(Error);
		}

		FEdGraphPinType PinType;

		if (VariableType == TEXT("Boolean")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		}
		else if (VariableType == TEXT("Integer") || VariableType == TEXT("Int")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
		}
		else if (VariableType == TEXT("Float")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Float;
		}
		else if (VariableType == TEXT("String")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_String;
		}
		else if (VariableType == TEXT("Vector")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
		}
		else {
			return FVoidResult::Failure(FString::Printf(TEXT("Unsupported variable type: %s"), *VariableType));
		}

		FBlueprintEditorUtils::AddMemberVariable(Blueprint, FName(*VariableName), PinType);

		if (bIsExposed) {
			for (FBPVariableDescription& Variable : Blueprint->NewVariables) {
				if (Variable.VarName == FName(*VariableName)) {
					Variable.PropertyFlags |= CPF_Edit;
					break;
				}
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return FVoidResult::Success();
	}

	UBlueprint* FBlueprintGraphService::FindBlueprint(const FString& BlueprintName, FString& OutError) {
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			OutError = FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName);
		}
		return Blueprint;
	}

	UEdGraph* FBlueprintGraphService::GetEventGraph(UBlueprint* Blueprint, FString& OutError) {
		UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(Blueprint);
		if (!EventGraph) {
			OutError = TEXT("Failed to get event graph");
		}
		return EventGraph;
	}

	UEdGraphNode* FBlueprintGraphService::FindNodeByGuid(UEdGraph* Graph, const FString& NodeGuid) {
		for (UEdGraphNode* Node : Graph->Nodes) {
			if (Node->NodeGuid.ToString() == NodeGuid) {
				return Node;
			}
		}
		return nullptr;
	}

	auto FBlueprintGraphService::SetFunctionParameters(
		UK2Node_CallFunction* FunctionNode,
		UEdGraph* EventGraph,
		const TSharedPtr<FJsonObject>& Parameters
	) -> FVoidResult {
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Param : Parameters->Values) {
			const FString& ParamName = Param.Key;
			const TSharedPtr<FJsonValue>& ParamValue = Param.Value;

			UEdGraphPin* ParamPin = FCommonUtils::FindPin(FunctionNode, ParamName, EGPD_Input);
			if (!ParamPin) {
				UE_LOG(LogBlueprintGraphService, Warning, TEXT("Parameter pin '%s' not found"), *ParamName);
				continue;
			}

			// Handle class reference parameters
			if (ParamPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Class && ParamValue->Type == EJson::String) {
				const FString& ClassName = ParamValue->AsString();
				UClass* Class = FindFirstObject<UClass>(*ClassName, EFindFirstObjectOptions::NativeFirst);

				if (!Class) {
					Class = LoadObject<UClass>(nullptr, *ClassName);
				}

				if (!Class) {
					FString EngineClassName = FString::Printf(TEXT("/Script/Engine.%s"), *ClassName);
					Class = LoadObject<UClass>(nullptr, *EngineClassName);
				}

				if (!Class) {
					return FVoidResult::Failure(FString::Printf(TEXT("Failed to find class '%s'"), *ClassName));
				}

				const UEdGraphSchema_K2* K2Schema = Cast<const UEdGraphSchema_K2>(EventGraph->GetSchema());
				if (!K2Schema) {
					return FVoidResult::Failure(TEXT("Failed to get K2Schema"));
				}

				K2Schema->TrySetDefaultObject(*ParamPin, Class);
			}
			// Handle numeric parameters
			else if (ParamValue->Type == EJson::Number) {
				if (ParamPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Int) {
					const int32 IntValue = FMath::RoundToInt(ParamValue->AsNumber());
					ParamPin->DefaultValue = FString::FromInt(IntValue);
				}
				else {
					const float FloatValue = ParamValue->AsNumber();
					ParamPin->DefaultValue = FString::SanitizeFloat(FloatValue);
				}
			}
			// Handle boolean parameters
			else if (ParamValue->Type == EJson::Boolean) {
				const bool BoolValue = ParamValue->AsBool();
				ParamPin->DefaultValue = BoolValue ? TEXT("true") : TEXT("false");
			}
			// Handle vector parameters
			else if (ParamValue->Type == EJson::Array) {
				const TArray<TSharedPtr<FJsonValue>>* ArrayValue;
				if (ParamValue->TryGetArray(ArrayValue) && ArrayValue->Num() == 3 &&
					ParamPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct &&
					ParamPin->PinType.PinSubCategoryObject == TBaseStructure<FVector>::Get()) {
					const float X = (*ArrayValue)[0]->AsNumber();
					const float Y = (*ArrayValue)[1]->AsNumber();
					const float Z = (*ArrayValue)[2]->AsNumber();
					ParamPin->DefaultValue = FString::Printf(TEXT("(X=%f,Y=%f,Z=%f)"), X, Y, Z);
				}
			}
			// Handle string parameters
			else if (ParamValue->Type == EJson::String) {
				ParamPin->DefaultValue = ParamValue->AsString();
			}
		}

		return FVoidResult::Success();
	}

} // namespace UnrealMCP
