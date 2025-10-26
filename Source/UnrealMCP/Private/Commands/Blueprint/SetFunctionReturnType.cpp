#include "Commands/Blueprint/SetFunctionReturnType.h"
#include "Core/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_FunctionResult.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FSetFunctionReturnTypeCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse required parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name")) ||
			!Params->HasField(TEXT("return_type"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name, function_name, and return_type")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));
		const FString ReturnType = Params->GetStringField(TEXT("return_type"));

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

		// Find the function result node (or create one if it doesn't exist)
		UK2Node_FunctionResult* ResultNode = nullptr;
		for (UEdGraphNode* Node : FunctionGraph->Nodes) {
			if (UK2Node_FunctionResult* Result = Cast<UK2Node_FunctionResult>(Node)) {
				ResultNode = Result;
				break;
			}
		}

		// If no result node exists and we're setting a return type, create one
		if (!ResultNode) {
			ResultNode = NewObject<UK2Node_FunctionResult>(FunctionGraph);
			if (ResultNode) {
				FunctionGraph->AddNode(ResultNode, false, false);
				ResultNode->CreateNewGuid();
				ResultNode->PostPlacedNewNode();
				ResultNode->AllocateDefaultPins();
			} else {
				return FCommonUtils::CreateErrorResponse(TEXT("Failed to create function result node"));
			}
		}

		// Create the pin type based on the return type
		FEdGraphPinType PinType;
		PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean; // Default

		if (ReturnType.Equals(TEXT("bool"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		} else if (ReturnType.Equals(TEXT("int"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
		} else if (ReturnType.Equals(TEXT("float"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
			PinType.PinSubCategory = UEdGraphSchema_K2::PC_Float;
		} else if (ReturnType.Equals(TEXT("string"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_String;
		} else if (ReturnType.Equals(TEXT("name"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Name;
		} else if (ReturnType.Equals(TEXT("vector"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
		} else if (ReturnType.Equals(TEXT("rotator"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
		} else if (ReturnType.Equals(TEXT("transform"), ESearchCase::IgnoreCase)) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FTransform>::Get();
		} else {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Unsupported return type: %s"), *ReturnType)
			);
		}

		// Add or update the return value pin
		const FName ReturnValueName = FName(TEXT("ReturnValue"));

		// Check if return value pin already exists
		bool bPinExists = false;
		for (UEdGraphPin* Pin : ResultNode->Pins) {
			if (Pin->PinName == ReturnValueName) {
				// Update existing pin type
				Pin->PinType = PinType;
				bPinExists = true;
				break;
			}
		}

		// If pin doesn't exist, add it
		if (!bPinExists) {
			TSharedPtr<FUserPinInfo> NewReturnPin = MakeShared<FUserPinInfo>();
			NewReturnPin->PinName = ReturnValueName;
			NewReturnPin->PinType = PinType;
			NewReturnPin->DesiredPinDirection = EGPD_Input; // Return values are inputs to the result node

			ResultNode->UserDefinedPins.Add(NewReturnPin);
		}

		// Reconstruct the node to apply changes
		ResultNode->ReconstructNode();

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(
					TEXT("Return type '%s' set for function '%s' in blueprint '%s'"),
					*ReturnType,
					*FunctionName,
					*BlueprintName
				)
			);
		});
	}

} // namespace UnrealMCP
