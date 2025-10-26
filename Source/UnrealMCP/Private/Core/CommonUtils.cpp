#include "Core/CommonUtils.h"
#include "Services/BlueprintIntrospectionService.h"
#include "GameFramework/Actor.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_InputAction.h"
#include "K2Node_Self.h"
#include "EdGraphSchema_K2.h"
#include "MCPPropertyHandlers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/UObjectIterator.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

// JSON Utilities
auto FCommonUtils::CreateErrorResponse(const FString& Message) -> TSharedPtr<FJsonObject> {
	TSharedPtr<FJsonObject> ResponseObject = MakeShared<FJsonObject>();
	ResponseObject->SetBoolField(TEXT("success"), false);
	ResponseObject->SetStringField(TEXT("error"), Message);
	return ResponseObject;
}

auto FCommonUtils::CreateSuccessResponse(const TSharedPtr<FJsonObject>& Data) -> TSharedPtr<FJsonObject> {
	TSharedPtr<FJsonObject> ResponseObject = MakeShared<FJsonObject>();
	ResponseObject->SetBoolField(TEXT("success"), true);

	if (Data.IsValid()) {
		ResponseObject->SetObjectField(TEXT("data"), Data);
	}

	return ResponseObject;
}

auto FCommonUtils::CreateSuccessResponse(
	const TFunction<void(TSharedPtr<FJsonObject>&)>& DataBuilder
) -> TSharedPtr<FJsonObject> {
	TSharedPtr<FJsonObject> ResponseObject = MakeShared<FJsonObject>();
	ResponseObject->SetBoolField(TEXT("success"), true);

	if (DataBuilder) {
		TSharedPtr<FJsonObject> Data = MakeShared<FJsonObject>();
		DataBuilder(Data);
		ResponseObject->SetObjectField(TEXT("data"), Data);
	}

	return ResponseObject;
}

void FCommonUtils::GetIntArrayFromJson(
	const TSharedPtr<FJsonObject>& JsonObject,
	const FString& FieldName,
	TArray<int32>& OutArray
) {
	OutArray.Reset();

	if (!JsonObject->HasField(FieldName)) {
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* JsonArray;
	if (JsonObject->TryGetArrayField(FieldName, JsonArray)) {
		for (const TSharedPtr<FJsonValue>& Value : *JsonArray) {
			OutArray.Add(static_cast<int32>(Value->AsNumber()));
		}
	}
}

void FCommonUtils::GetFloatArrayFromJson(
	const TSharedPtr<FJsonObject>& JsonObject,
	const FString& FieldName,
	TArray<float>& OutArray
) {
	OutArray.Reset();

	if (!JsonObject->HasField(FieldName)) {
		return;
	}

	if (const TArray<TSharedPtr<FJsonValue>>* JsonArray; JsonObject->TryGetArrayField(FieldName, JsonArray)) {
		for (const TSharedPtr<FJsonValue>& Value : *JsonArray) {
			OutArray.Add(static_cast<float>(Value->AsNumber()));
		}
	}
}

auto FCommonUtils::GetVector2DFromJson(const TSharedPtr<FJsonObject>& JsonObject,
                                       const FString& FieldName) -> FVector2D {
	FVector2D Result(0.0f, 0.0f);

	if (!JsonObject->HasField(FieldName)) {
		return Result;
	}

	if (const TArray<TSharedPtr<FJsonValue>>* JsonArray; JsonObject->TryGetArrayField(FieldName, JsonArray) && JsonArray
		->Num() >= 2) {
		Result.X = static_cast<float>((*JsonArray)[0]->AsNumber());
		Result.Y = static_cast<float>((*JsonArray)[1]->AsNumber());
	}

	return Result;
}

auto FCommonUtils::GetVectorFromJson(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) -> FVector {
	FVector Result(0.0f, 0.0f, 0.0f);

	if (!JsonObject->HasField(FieldName)) {
		return Result;
	}

	if (const TArray<TSharedPtr<FJsonValue>>* JsonArray; JsonObject->TryGetArrayField(FieldName, JsonArray) && JsonArray
		->Num() >= 3) {
		Result.X = static_cast<float>((*JsonArray)[0]->AsNumber());
		Result.Y = static_cast<float>((*JsonArray)[1]->AsNumber());
		Result.Z = static_cast<float>((*JsonArray)[2]->AsNumber());
	}

	return Result;
}

auto FCommonUtils::GetRotatorFromJson(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) -> FRotator {
	FRotator Result(0.0f, 0.0f, 0.0f);

	if (!JsonObject->HasField(FieldName)) {
		return Result;
	}

	if (const TArray<TSharedPtr<FJsonValue>>* JsonArray; JsonObject->TryGetArrayField(FieldName, JsonArray) && JsonArray
		->Num() >= 3) {
		Result.Pitch = static_cast<float>((*JsonArray)[0]->AsNumber());
		Result.Yaw = static_cast<float>((*JsonArray)[1]->AsNumber());
		Result.Roll = static_cast<float>((*JsonArray)[2]->AsNumber());
	}

	return Result;
}

/**  Actor utilities */
auto FCommonUtils::ActorToJson(const AActor* Actor) -> TSharedPtr<FJsonValue> {
	if (!Actor) {
		return MakeShared<FJsonValueNull>();
	}

	TSharedPtr<FJsonObject> ActorObject = MakeShared<FJsonObject>();
	ActorObject->SetStringField(TEXT("name"), Actor->GetName());
	ActorObject->SetStringField(TEXT("class"), Actor->GetClass()->GetName());

	FVector Location = Actor->GetActorLocation();
	TArray<TSharedPtr<FJsonValue>> LocationArray;
	LocationArray.Add(MakeShared<FJsonValueNumber>(Location.X));
	LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Y));
	LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Z));
	ActorObject->SetArrayField(TEXT("location"), LocationArray);

	FRotator Rotation = Actor->GetActorRotation();
	TArray<TSharedPtr<FJsonValue>> RotationArray;
	RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Pitch));
	RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Yaw));
	RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Roll));
	ActorObject->SetArrayField(TEXT("rotation"), RotationArray);

	FVector Scale = Actor->GetActorScale3D();
	TArray<TSharedPtr<FJsonValue>> ScaleArray;
	ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.X));
	ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Y));
	ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Z));
	ActorObject->SetArrayField(TEXT("scale"), ScaleArray);

	return MakeShared<FJsonValueObject>(ActorObject);
}

auto FCommonUtils::ActorToJsonObject(const AActor* Actor, [[maybe_unused]] bool bDetailed) -> TSharedPtr<FJsonObject> {
	if (!Actor) {
		return nullptr;
	}

	TSharedPtr<FJsonObject> ActorObject = MakeShared<FJsonObject>();
	ActorObject->SetStringField(TEXT("name"), Actor->GetName());
	ActorObject->SetStringField(TEXT("class"), Actor->GetClass()->GetName());

	FVector Location = Actor->GetActorLocation();
	TArray<TSharedPtr<FJsonValue>> LocationArray;
	LocationArray.Add(MakeShared<FJsonValueNumber>(Location.X));
	LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Y));
	LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Z));
	ActorObject->SetArrayField(TEXT("location"), LocationArray);

	FRotator Rotation = Actor->GetActorRotation();
	TArray<TSharedPtr<FJsonValue>> RotationArray;
	RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Pitch));
	RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Yaw));
	RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Roll));
	ActorObject->SetArrayField(TEXT("rotation"), RotationArray);

	FVector Scale = Actor->GetActorScale3D();
	TArray<TSharedPtr<FJsonValue>> ScaleArray;
	ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.X));
	ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Y));
	ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Z));
	ActorObject->SetArrayField(TEXT("scale"), ScaleArray);

	return ActorObject;
}

// Blueprint Utilities
auto FCommonUtils::FindBlueprint(const FString& BlueprintName) -> UBlueprint* {
	return FindBlueprintByName(BlueprintName);
}

auto FCommonUtils::FindBlueprintByName(const FString& BlueprintName) -> UBlueprint* {
	// Use the introspection service to find the blueprint path
	const FString BlueprintPath = UnrealMCP::FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName);
	if (BlueprintPath.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Blueprint '%s' not found"), *BlueprintName);
		return nullptr;
	}

	UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *BlueprintPath);
	if (!Blueprint) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load blueprint '%s' from path: %s"), *BlueprintName, *BlueprintPath);
		return nullptr;
	}

	UE_LOG(LogTemp, Display, TEXT("Found blueprint '%s' at: %s"), *BlueprintName, *BlueprintPath);
	return Blueprint;
}

UEdGraph* FCommonUtils::FindOrCreateEventGraph(UBlueprint* Blueprint) {
	if (!Blueprint) {
		return nullptr;
	}

	// Try to find the event graph
	for (UEdGraph* Graph : Blueprint->UbergraphPages) {
		if (Graph->GetName().Contains(TEXT("EventGraph"))) {
			return Graph;
		}
	}

	// Create a new event graph if none exists
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint,
	                                                           FName(TEXT("EventGraph")),
	                                                           UEdGraph::StaticClass(),
	                                                           UEdGraphSchema_K2::StaticClass());
	FBlueprintEditorUtils::AddUbergraphPage(Blueprint, NewGraph);
	return NewGraph;
}

// Pin type utilities
auto FCommonUtils::ParsePinType(const FString& TypeString, FEdGraphPinType& OutPinType) -> bool {
	if (TypeString.Equals(TEXT("bool"), ESearchCase::IgnoreCase) ||
	    TypeString.Equals(TEXT("boolean"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	}
	else if (TypeString.Equals(TEXT("int"), ESearchCase::IgnoreCase) ||
	         TypeString.Equals(TEXT("integer"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Int;
	}
	else if (TypeString.Equals(TEXT("float"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Real;
		OutPinType.PinSubCategory = UEdGraphSchema_K2::PC_Float;
	}
	else if (TypeString.Equals(TEXT("string"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_String;
	}
	else if (TypeString.Equals(TEXT("name"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Name;
	}
	else if (TypeString.Equals(TEXT("vector"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutPinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
	}
	else if (TypeString.Equals(TEXT("rotator"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutPinType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
	}
	else if (TypeString.Equals(TEXT("transform"), ESearchCase::IgnoreCase)) {
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutPinType.PinSubCategoryObject = TBaseStructure<FTransform>::Get();
	}
	else {
		return false;
	}

	return true;
}

// Blueprint node utilities
UK2Node_Event* FCommonUtils::CreateEventNode(UEdGraph* Graph, const FString& EventName, const FVector2D& Position) {
	if (!Graph) {
		return nullptr;
	}

	const UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
	if (!Blueprint) {
		return nullptr;
	}

	// Check for existing event node with this exact name
	for (UEdGraphNode* Node : Graph->Nodes) {
		if (UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node); EventNode && EventNode->EventReference.GetMemberName()
			== FName(*EventName)) {
			UE_LOG(LogTemp,
			       Display,
			       TEXT("Using existing event node with name %s (ID: %s)"),
			       *EventName,
			       *EventNode->NodeGuid.ToString());
			return EventNode;
		}
	}

	// No existing node found, create a new one
	UK2Node_Event* EventNode = nullptr;

	if (UClass* BlueprintClass = Blueprint->GeneratedClass; BlueprintClass->FindFunctionByName(FName(*EventName))) {
		EventNode = NewObject<UK2Node_Event>(Graph);
		EventNode->EventReference.SetExternalMember(FName(*EventName), BlueprintClass);
		EventNode->NodePosX = Position.X;
		EventNode->NodePosY = Position.Y;
		Graph->AddNode(EventNode, true);
		EventNode->PostPlacedNewNode();
		EventNode->AllocateDefaultPins();
		UE_LOG(LogTemp,
		       Display,
		       TEXT("Created new event node with name %s (ID: %s)"),
		       *EventName,
		       *EventNode->NodeGuid.ToString());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to find function for event name: %s"), *EventName);
	}

	return EventNode;
}

UK2Node_CallFunction* FCommonUtils::CreateFunctionCallNode(UEdGraph* Graph,
                                                           const UFunction* Function,
                                                           const FVector2D& Position) {
	if (!Graph || !Function) {
		return nullptr;
	}

	UK2Node_CallFunction* FunctionNode = NewObject<UK2Node_CallFunction>(Graph);
	FunctionNode->SetFromFunction(Function);
	FunctionNode->NodePosX = Position.X;
	FunctionNode->NodePosY = Position.Y;
	Graph->AddNode(FunctionNode, true);
	FunctionNode->CreateNewGuid();
	FunctionNode->PostPlacedNewNode();
	FunctionNode->AllocateDefaultPins();

	return FunctionNode;
}

UK2Node_VariableGet* FCommonUtils::CreateVariableGetNode(UEdGraph* Graph,
                                                         const UBlueprint* Blueprint,
                                                         const FString& VariableName,
                                                         const FVector2D& Position) {
	if (!Graph || !Blueprint) {
		return nullptr;
	}

	UK2Node_VariableGet* VariableGetNode = NewObject<UK2Node_VariableGet>(Graph);

	const FName VarName(*VariableName);

	if (const FProperty* Property = FindFProperty<FProperty>(Blueprint->GeneratedClass, VarName)) {
		VariableGetNode->VariableReference.SetFromField<FProperty>(Property, false);
		VariableGetNode->NodePosX = Position.X;
		VariableGetNode->NodePosY = Position.Y;
		Graph->AddNode(VariableGetNode, true);
		VariableGetNode->PostPlacedNewNode();
		VariableGetNode->AllocateDefaultPins();

		return VariableGetNode;
	}

	return nullptr;
}

UK2Node_VariableSet* FCommonUtils::CreateVariableSetNode(UEdGraph* Graph,
                                                         const UBlueprint* Blueprint,
                                                         const FString& VariableName,
                                                         const FVector2D& Position) {
	if (!Graph || !Blueprint) {
		return nullptr;
	}

	UK2Node_VariableSet* VariableSetNode = NewObject<UK2Node_VariableSet>(Graph);

	const FName VarName(*VariableName);

	if (const FProperty* Property = FindFProperty<FProperty>(Blueprint->GeneratedClass, VarName)) {
		VariableSetNode->VariableReference.SetFromField<FProperty>(Property, false);
		VariableSetNode->NodePosX = Position.X;
		VariableSetNode->NodePosY = Position.Y;
		Graph->AddNode(VariableSetNode, true);
		VariableSetNode->PostPlacedNewNode();
		VariableSetNode->AllocateDefaultPins();

		return VariableSetNode;
	}

	return nullptr;
}

UK2Node_InputAction* FCommonUtils::CreateInputActionNode(UEdGraph* Graph,
                                                         const FString& ActionName,
                                                         const FVector2D& Position) {
	if (!Graph) {
		return nullptr;
	}

	UK2Node_InputAction* InputActionNode = NewObject<UK2Node_InputAction>(Graph);
	InputActionNode->InputActionName = FName(*ActionName);
	InputActionNode->NodePosX = Position.X;
	InputActionNode->NodePosY = Position.Y;
	Graph->AddNode(InputActionNode, true);
	InputActionNode->CreateNewGuid();
	InputActionNode->PostPlacedNewNode();
	InputActionNode->AllocateDefaultPins();

	return InputActionNode;
}

UK2Node_Self* FCommonUtils::CreateSelfReferenceNode(UEdGraph* Graph, const FVector2D& Position) {
	if (!Graph) {
		return nullptr;
	}

	UK2Node_Self* SelfNode = NewObject<UK2Node_Self>(Graph);
	SelfNode->NodePosX = Position.X;
	SelfNode->NodePosY = Position.Y;
	Graph->AddNode(SelfNode, true);
	SelfNode->CreateNewGuid();
	SelfNode->PostPlacedNewNode();
	SelfNode->AllocateDefaultPins();

	return SelfNode;
}

bool FCommonUtils::ConnectGraphNodes(const UEdGraph* Graph,
                                     UEdGraphNode* SourceNode,
                                     const FString& SourcePinName,
                                     UEdGraphNode* TargetNode,
                                     const FString& TargetPinName) {
	if (!Graph || !SourceNode || !TargetNode) {
		return false;
	}

	UEdGraphPin* SourcePin = FindPin(SourceNode, SourcePinName, EGPD_Output);
	UEdGraphPin* TargetPin = FindPin(TargetNode, TargetPinName, EGPD_Input);

	if (SourcePin && TargetPin) {
		SourcePin->MakeLinkTo(TargetPin);
		return true;
	}

	return false;
}

UEdGraphPin* FCommonUtils::FindPin(UEdGraphNode* Node, const FString& PinName, const EEdGraphPinDirection Direction) {
	if (!Node) {
		return nullptr;
	}

	// Log all pins for debugging
	UE_LOG(LogTemp,
	       Display,
	       TEXT("FindPin: Looking for pin '%s' (Direction: %d) in node '%s'"),
	       *PinName,
	       (int32)Direction,
	       *Node->GetName());

	for (const UEdGraphPin* Pin : Node->Pins) {
		UE_LOG(LogTemp,
		       Display,
		       TEXT("  - Available pin: '%s', Direction: %d, Category: %s"),
		       *Pin->PinName.ToString(),
		       (int32)Pin->Direction,
		       *Pin->PinType.PinCategory.ToString());
	}

	// First try exact match
	for (UEdGraphPin* Pin : Node->Pins) {
		if (Pin->PinName.ToString() == PinName && (Direction == EGPD_MAX || Pin->Direction == Direction)) {
			UE_LOG(LogTemp, Display, TEXT("  - Found exact matching pin: '%s'"), *Pin->PinName.ToString());
			return Pin;
		}
	}

	// If no exact match and we're looking for a component reference, try case-insensitive match
	for (UEdGraphPin* Pin : Node->Pins) {
		if (Pin->PinName.ToString().Equals(PinName, ESearchCase::IgnoreCase) &&
			(Direction == EGPD_MAX || Pin->Direction == Direction)) {
			UE_LOG(LogTemp, Display, TEXT("  - Found case-insensitive matching pin: '%s'"), *Pin->PinName.ToString());
			return Pin;
		}
	}

	// If we're looking for a component output and didn't find it by name, try to find the first data output pin
	if (Direction == EGPD_Output && Cast<UK2Node_VariableGet>(Node) != nullptr) {
		for (UEdGraphPin* Pin : Node->Pins) {
			if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec) {
				UE_LOG(LogTemp, Display, TEXT("  - Found fallback data output pin: '%s'"), *Pin->PinName.ToString());
				return Pin;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  - No matching pin found for '%s'"), *PinName);
	return nullptr;
}

UK2Node_Event* FCommonUtils::FindExistingEventNode(UEdGraph* Graph, const FString& EventName) {
	if (!Graph) {
		return nullptr;
	}

	// Look for existing event nodes
	for (UEdGraphNode* Node : Graph->Nodes) {
		UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
		if (EventNode && EventNode->EventReference.GetMemberName() == FName(*EventName)) {
			UE_LOG(LogTemp, Display, TEXT("Found existing event node with name: %s"), *EventName);
			return EventNode;
		}
	}

	return nullptr;
}

bool FCommonUtils::SetObjectProperty(
	UObject* Object,
	const FString& PropertyName,
	const TSharedPtr<FJsonValue>& Value,
	FString& OutErrorMessage
) {
	if (!Object) {
		OutErrorMessage = TEXT("Invalid object");
		return false;
	}

	FProperty* Property = Object->GetClass()->FindPropertyByName(*PropertyName);
	if (!Property) {
		OutErrorMessage = FString::Printf(TEXT("Property not found: %s"), *PropertyName);
		return false;
	}

	void* PropertyAddr = Property->ContainerPtrToValuePtr<void>(Object);

	PropertyHandlers::FPropertyHandler Handler = [&]() -> PropertyHandlers::FPropertyHandler {
		if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Property)) {
			return PropertyHandlers::FBoolHandler{BoolProp, PropertyAddr};
		}

		if (const FIntProperty* IntProp = CastField<FIntProperty>(Property)) {
			return PropertyHandlers::FIntHandler{IntProp, Object};
		}

		if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Property)) {
			return PropertyHandlers::FFloatHandler{FloatProp, PropertyAddr};
		}

		if (const FStrProperty* StrProp = CastField<FStrProperty>(Property)) {
			return PropertyHandlers::FStringHandler{StrProp, PropertyAddr};
		}

		if (const FByteProperty* ByteProp = CastField<FByteProperty>(Property)) {
			if (const UEnum* EnumDef = ByteProp->GetIntPropertyEnum()) {
				return PropertyHandlers::FEnumByteHandler{ByteProp, EnumDef, PropertyAddr, PropertyName};
			}
			return PropertyHandlers::FByteHandler{ByteProp, PropertyAddr, PropertyName};
		}

		if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Property)) {
			const UEnum* EnumDef = EnumProp->GetEnum();
			const FNumericProperty* NumericProp = EnumProp->GetUnderlyingProperty();

			if (EnumDef && NumericProp) {
				return PropertyHandlers::FEnumHandler{EnumProp, EnumDef, NumericProp, PropertyAddr, PropertyName};
			}
		}

		return PropertyHandlers::FUnsupportedHandler{Property->GetClass()->GetName(), PropertyName};
	}();

	return std::visit(PropertyHandlers::FExecuteHandler{Value, OutErrorMessage}, Handler);
}
