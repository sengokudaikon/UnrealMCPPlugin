#include "Commands/Blueprint/AddFunctionParameter.h"

#include "Core/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_FunctionEntry.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FAddFunctionParameterCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse required parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name")) ||
			!Params->HasField(TEXT("param_name")) || !Params->HasField(TEXT("param_type"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name, function_name, param_name, and param_type")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));
		const FString ParamName = Params->GetStringField(TEXT("param_name"));
		const FString ParamType = Params->GetStringField(TEXT("param_type"));
		const bool bIsReference = Params->HasField(TEXT("is_reference"))
			? Params->GetBoolField(TEXT("is_reference"))
			: false;

		// Find the blueprint
		UBlueprint* Blueprint =
			LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName));
		if (!Blueprint) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the function graph
		const FName FunctionFName = FName(*FunctionName);
		UEdGraph* FunctionGraph = nullptr;

		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (Graph->GetFName() == FunctionFName) {
				FunctionGraph = Graph;
				break;
			}
		}

		if (!FunctionGraph) {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Function '%s' not found in blueprint"), *FunctionName)
			);
		}

		// Find the function entry node
		UK2Node_FunctionEntry* EntryNode = nullptr;
		for (UEdGraphNode* Node : FunctionGraph->Nodes) {
			if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(Node)) {
				EntryNode = Entry;
				break;
			}
		}

		if (!EntryNode) {
			return FCommonUtils::CreateErrorResponse(TEXT("Function entry node not found"));
		}

		// Create the pin type based on the parameter type
		FEdGraphPinType PinType;
		PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean; // Default

		if (ParamType.Equals(TEXT("bool"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		}
		else if (ParamType.Equals(TEXT("int"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
		}
		else if (ParamType.Equals(TEXT("float"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
			PinType.PinSubCategory = UEdGraphSchema_K2::PC_Float;
		}
		else if (ParamType.Equals(TEXT("string"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_String;
		}
		else if (ParamType.Equals(TEXT("name"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Name;
		}
		else if (ParamType.Equals(TEXT("vector"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
		}
		else if (ParamType.Equals(TEXT("rotator"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
		}
		else if (ParamType.Equals(TEXT("transform"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FTransform>::Get();
		}
		else {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Unsupported parameter type: %s"), *ParamType)
			);
		}

		// Set reference flag if needed
		if (bIsReference) {
			PinType.bIsReference = true;
		}

		// Add the parameter using user defined pins
		const auto ParamFName = FName(*ParamName);

		// Create a user defined pin for the parameter
		TSharedPtr<FUserPinInfo> NewParam = MakeShared<FUserPinInfo>();
		NewParam->PinName = ParamFName;
		NewParam->PinType = PinType;
		NewParam->DesiredPinDirection = EGPD_Output; // Parameters are outputs from the entry node

		EntryNode->UserDefinedPins.Add(NewParam);
		EntryNode->ReconstructNode();

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(
					TEXT("Parameter '%s' of type '%s' added to function '%s' in blueprint '%s'"),
					*ParamName,
					*ParamType,
					*FunctionName,
					*BlueprintName
				)
			);
		});
	}

} // namespace UnrealMCP
