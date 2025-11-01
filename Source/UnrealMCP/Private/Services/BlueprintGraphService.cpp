#include "Services/BlueprintGraphService.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"
#include "K2Node_InputAction.h"
#include "K2Node_Self.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Camera/CameraActor.h"
#include "Core/CommonUtils.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet2/BlueprintEditorUtils.h"

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
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}
		if (SourceNodeId.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Source node ID cannot be empty"));
		}
		if (TargetNodeId.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Target node ID cannot be empty"));
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
			return FVoidResult::Failure(EErrorCode::NodeNotFound, TEXT("Source or target node not found"));
		}

		if (FCommonUtils::ConnectGraphNodes(EventGraph, SourceNode, SourcePinName, TargetNode, TargetPinName)) {
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
			return FVoidResult::Success();
		}

		return FVoidResult::Failure(EErrorCode::NodeConnectionFailed, TEXT("Failed to connect nodes"));
	}

	auto FBlueprintGraphService::AddEventNode(
		const FString& BlueprintName,
		const FString& EventName,
		const FVector2D& NodePosition
	) -> TResult<UK2Node_Event*> {
		// Validate input parameters
		if (BlueprintName.IsEmpty()) {
			return TResult<UK2Node_Event*>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}
		if (EventName.IsEmpty()) {
			return TResult<UK2Node_Event*>::Failure(EErrorCode::InvalidInput, TEXT("Event name cannot be empty"));
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

	UClass* EventSourceClass = nullptr;
	const UFunction* EventFunction = nullptr;

	if (UClass* BlueprintClass = Blueprint->GeneratedClass) {
		// First check the blueprint class itself
		EventFunction = BlueprintClass->FindFunctionByName(FName(*EventName));
		if (EventFunction) {
			EventSourceClass = BlueprintClass;
		}

		if (!EventFunction) {
			for (UClass* ParentClass = BlueprintClass->GetSuperClass(); ParentClass; ParentClass = ParentClass->GetSuperClass()) {
				EventFunction = ParentClass->FindFunctionByName(FName(*EventName));
				if (EventFunction) {
					EventSourceClass = ParentClass;
					break;
				}
			}
		}


		if (!EventFunction && BlueprintClass->IsChildOf(AActor::StaticClass())) {
			UE_LOG(LogTemp, Display, TEXT("Attempting fallback mapping for event: %s"), *EventName);
			// Map common event names to their actual function names
			TMap<FString, FString> EventNameMapping = {
				{TEXT("BeginPlay"), TEXT("ReceiveBeginPlay")},
				{TEXT("EndPlay"), TEXT("ReceiveEndPlay")},
				{TEXT("ActorBeginPlay"), TEXT("ReceiveBeginPlay")},  // ActorBeginPlay is the same as BeginPlay
				{TEXT("Tick"), TEXT("ReceiveTick")},
				{TEXT("ReceiveBeginPlay"), TEXT("ReceiveBeginPlay")},
				{TEXT("ReceiveEndPlay"), TEXT("ReceiveEndPlay")},
				{TEXT("ReceiveTick"), TEXT("ReceiveTick")}
			};

			// Try both the original name and mapped names
			TArray<FString> NamesToTry = {EventName};
			if (EventNameMapping.Contains(EventName)) {
				NamesToTry.Add(EventNameMapping[EventName]);
			}

			for (const FString& NameToTry : NamesToTry) {
				UE_LOG(LogTemp, Display, TEXT("Trying to find function: %s (for event: %s)"), *NameToTry, *EventName);
				EventFunction = AActor::StaticClass()->FindFunctionByName(FName(*NameToTry));
				if (EventFunction) {
					EventSourceClass = AActor::StaticClass();
					UE_LOG(LogTemp, Display, TEXT("Found common Actor event '%s' using fallback method (tried: %s)"), *EventName, *NameToTry);
					break;
				}
			}
		}
	}

	if (!EventSourceClass || !EventFunction) {
		// Enhanced error logging for debugging
		const FString ClassName = Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetName() : TEXT("None");
		const FString ParentClassName = Blueprint->ParentClass ? Blueprint->ParentClass->GetName() : TEXT("None");
		UE_LOG(LogTemp, Warning, TEXT("Failed to find function for event name: %s in class hierarchy (Blueprint: %s, Parent: %s)"),
		       *EventName, *ClassName, *ParentClassName);

		return TResult<UK2Node_Event*>::Failure(
			EErrorCode::FunctionNotFound,
			FString::Printf(TEXT("Event '%s' not found in blueprint '%s' class hierarchy. The event must exist in the blueprint class or its parent classes."), *EventName, *BlueprintName)
		);
	}

	// Check for existing event node with this exact function name
	const FName ActualFunctionName = EventFunction->GetFName();
	for (UEdGraphNode* Node : EventGraph->Nodes) {
		if (UK2Node_Event* ExistingEventNode = Cast<UK2Node_Event>(Node);
			ExistingEventNode && ExistingEventNode->EventReference.GetMemberName() == ActualFunctionName) {
			// Update position of existing node
			ExistingEventNode->NodePosX = NodePosition.X;
			ExistingEventNode->NodePosY = NodePosition.Y;
			UE_LOG(LogTemp, Display, TEXT("Using existing event node with name %s (function: %s, ID: %s), updated position to (%.1f, %.1f)"),
			       *EventName, *ActualFunctionName.ToString(), *ExistingEventNode->NodeGuid.ToString(), NodePosition.X, NodePosition.Y);
			return TResult<UK2Node_Event*>::Success(ExistingEventNode);
		}
	}

	// No existing node found, create a new one
	UK2Node_Event* EventNode = NewObject<UK2Node_Event>(EventGraph);
	// Use the actual function name, not the requested event name
	EventNode->EventReference.SetExternalMember(ActualFunctionName, EventSourceClass);
	EventNode->NodePosX = NodePosition.X;
	EventNode->NodePosY = NodePosition.Y;
	EventGraph->AddNode(EventNode, true);
	EventNode->PostPlacedNewNode();
	EventNode->AllocateDefaultPins();

	UE_LOG(LogTemp, Display, TEXT("Created new event node with name %s (function: %s, ID: %s) from class %s"),
	       *EventName, *ActualFunctionName.ToString(), *EventNode->NodeGuid.ToString(), *EventSourceClass->GetName());

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
			return TResult<UK2Node_CallFunction*>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}
		if (FunctionName.IsEmpty()) {
			return TResult<UK2Node_CallFunction*>::Failure(EErrorCode::InvalidInput, TEXT("Function name cannot be empty"));
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
		const UFunction* Function = nullptr;
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

			// Special case for UGameplayStatics and GameplayStatics
			if (!ClassPtr && (TargetClass.GetValue() == TEXT("UGameplayStatics") || TargetClass.GetValue() == TEXT("GameplayStatics"))) {
				ClassPtr = FindFirstObject<UClass>(TEXT("UGameplayStatics"), EFindFirstObjectOptions::NativeFirst);
				if (!ClassPtr) {
					ClassPtr = LoadObject<UClass>(nullptr, TEXT("/Script/Engine.GameplayStatics"));
				}
			}

			if (ClassPtr) {
				Function = ClassPtr->FindFunctionByName(*FunctionName);

				// If PrintString not found, try some common alternatives
				if (!Function && FunctionName == TEXT("PrintString")) {
					Function = ClassPtr->FindFunctionByName(FName(TEXT("PrintString")));
					if (!Function) {
						// Try to find any function with "Print" in the name but make sure it's the right one
						for (TFieldIterator<UFunction> It(ClassPtr); It; ++It) {
							const UFunction* Func = *It;
							if (Func->GetName().Contains(TEXT("Print")) && Func->GetName().Contains(TEXT("String"))) {
								Function = Func;
								break;
							}
						}
					}
				}
			}
		}

		// If we still haven't found the function, try in the blueprint's class
		if (!Function) {
			Function = Blueprint->GeneratedClass->FindFunctionByName(*FunctionName);
		}

		// For PrintString, try to find it in KismetSystemLibrary as a last resort
		if (!Function && FunctionName == TEXT("PrintString")) {
			const UClass* KismetSystemLibrary = LoadObject<UClass>(nullptr, TEXT("/Script/Engine.KismetSystemLibrary"));
			if (KismetSystemLibrary) {
				Function = KismetSystemLibrary->FindFunctionByName(*FunctionName);
			}
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
			return TResult<UK2Node_VariableGet*>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}
		if (ComponentName.IsEmpty()) {
			return TResult<UK2Node_VariableGet*>::Failure(EErrorCode::InvalidInput, TEXT("Component name cannot be empty"));
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
			return TResult<UK2Node_VariableGet*>::Failure(EErrorCode::NodeCreationFailed, TEXT("Failed to create get component node"));
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
			return TResult<UK2Node_Self*>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
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
			return TResult<UK2Node_Self*>::Failure(EErrorCode::NodeCreationFailed, TEXT("Failed to create self node"));
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
			return TResult<UK2Node_InputAction*>::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}
		if (ActionName.IsEmpty()) {
			return TResult<UK2Node_InputAction*>::Failure(EErrorCode::InvalidInput, TEXT("Action name cannot be empty"));
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

		UK2Node_InputAction* InputActionNode =
			FCommonUtils::CreateInputActionNode(EventGraph, ActionName, NodePosition);
		if (!InputActionNode) {
			return TResult<UK2Node_InputAction*>::Failure(EErrorCode::NodeCreationFailed, TEXT("Failed to create input action node"));
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
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}
		if (NodeType.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Node type cannot be empty"));
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

		// Performance logging
		const double StartTime = FPlatformTime::Seconds();
		int32 NodeCount = 0;
		UE_LOG(LogBlueprintGraphService, Display, TEXT("Starting node search: type='%s', blueprint='%s', total nodes=%d"),
		       *NodeType, *BlueprintName, EventGraph->Nodes.Num());

		if (NodeType == TEXT("Event")) {
			if (!EventName.IsSet()) {
				return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Missing 'event_name' parameter for Event node search"));
			}

			const FString EventNameStr = EventName.GetValue();
			const FName TargetEventName = FName(*EventNameStr);
			// Also try with "Receive" prefix for common events (BeginPlay -> ReceiveBeginPlay)
			const FName ReceiveEventName = EventNameStr.StartsWith(TEXT("Receive"))
				? TargetEventName
				: FName(*FString::Printf(TEXT("Receive%s"), *EventNameStr));

			for (UEdGraphNode* Node : EventGraph->Nodes) {
				NodeCount++;
				// Early exit for large graphs to prevent timeout
				if (NodeCount > 10000) {
					UE_LOG(LogBlueprintGraphService, Warning, TEXT("Node search truncated at 10000 nodes to prevent timeout"));
					break;
				}

				const UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
				if (EventNode) {
					const FName NodeEventName = EventNode->EventReference.GetMemberName();
					if (NodeEventName == TargetEventName || NodeEventName == ReceiveEventName) {
						OutNodeGuids.Add(EventNode->NodeGuid.ToString());
					}
				}
			}
		}
		else if (NodeType == TEXT("Function")) {
			for (UEdGraphNode* Node : EventGraph->Nodes) {
				NodeCount++;
				if (NodeCount > 10000) {
					UE_LOG(LogBlueprintGraphService, Warning, TEXT("Node search truncated at 10000 nodes to prevent timeout"));
					break;
				}

				if (const UK2Node_CallFunction* FunctionNode = Cast<UK2Node_CallFunction>(Node)) {
					OutNodeGuids.Add(FunctionNode->NodeGuid.ToString());
				}
			}
		}
		else if (NodeType == TEXT("Variable")) {
			for (UEdGraphNode* Node : EventGraph->Nodes) {
				NodeCount++;
				if (NodeCount > 10000) {
					UE_LOG(LogBlueprintGraphService, Warning, TEXT("Node search truncated at 10000 nodes to prevent timeout"));
					break;
				}

				if (Cast<UK2Node_VariableGet>(Node) || Cast<UK2Node_VariableSet>(Node)) {
					OutNodeGuids.Add(Node->NodeGuid.ToString());
				}
			}
		}
		else {
			return FVoidResult::Failure(EErrorCode::InvalidInput,
				FString::Printf(TEXT("Unsupported node type '%s'. Supported types: Event, Function, Variable"), *NodeType));
		}

		const double ElapsedTime = FPlatformTime::Seconds() - StartTime;
		UE_LOG(LogBlueprintGraphService, Display, TEXT("Node search completed: found %d nodes, searched %d nodes in %.3f seconds"),
		       OutNodeGuids.Num(), NodeCount, ElapsedTime);

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
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Blueprint name cannot be empty"));
		}
		if (VariableName.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Variable name cannot be empty"));
		}
		if (VariableType.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("Variable type cannot be empty"));
		}

		FString Error;
		UBlueprint* Blueprint = FindBlueprint(BlueprintName, Error);
		if (!Blueprint) {
			return FVoidResult::Failure(Error);
		}

		FEdGraphPinType PinType;
		if (!FCommonUtils::ParsePinType(VariableType, PinType)) {
			return FVoidResult::Failure(FString::Printf(TEXT("Unsupported variable type: %s"), *VariableType));
		}

		FBlueprintEditorUtils::AddMemberVariable(Blueprint, FName(*VariableName), PinType);

		// Set or clear CPF_Edit flag based on bIsExposed parameter
		for (FBPVariableDescription& Variable : Blueprint->NewVariables) {
			if (Variable.VarName == FName(*VariableName)) {
				if (bIsExposed) {
					Variable.PropertyFlags |= CPF_Edit;
				} else {
					Variable.PropertyFlags &= ~CPF_Edit;
				}
				break;
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return FVoidResult::Success();
	}

	auto FBlueprintGraphService::FindBlueprint(const FString& BlueprintName, FString& OutError) -> UBlueprint* {
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			OutError = FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName);
		}
		return Blueprint;
	}

	auto FBlueprintGraphService::GetEventGraph(UBlueprint* Blueprint, FString& OutError) -> UEdGraph* {
		UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(Blueprint);
		if (!EventGraph) {
			OutError = TEXT("Failed to get event graph");
		}
		return EventGraph;
	}

	auto FBlueprintGraphService::FindNodeByGuid(UEdGraph* Graph, const FString& NodeGuid) -> UEdGraphNode* {
		for (UEdGraphNode* Node : Graph->Nodes) {
			if (Node->NodeGuid.ToString() == NodeGuid) {
				return Node;
			}
		}
		return nullptr;
	}

	auto FBlueprintGraphService::SetFunctionParameters(
		UK2Node_CallFunction* FunctionNode,
		const UEdGraph* EventGraph,
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
					return FVoidResult::Failure(EErrorCode::OperationFailed, TEXT("Failed to get K2Schema"));
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

}
