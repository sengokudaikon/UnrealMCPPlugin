#include "Services/BlueprintMemberService.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "Core/CommonUtils.h"
#include "EdGraph/EdGraph.h"
#include "Engine/Blueprint.h"
#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

namespace UnrealMCP {

	// ============ Function Operations ============

	auto FBlueprintMemberService::AddFunction(
		const FString& BlueprintName,
		const FString& FunctionName
	) -> TResult<FString> {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return TResult<FString>::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Check if function already exists
		const auto FunctionFName = FName(*FunctionName);
		for (const UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (Graph->GetFName() == FunctionFName) {
				return TResult<FString>::Failure(
					FString::Printf(TEXT("Function '%s' already exists in blueprint"), *FunctionName)
				);
			}
		}

		// Create the function graph
		UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(
			Blueprint,
			FunctionFName,
			UEdGraph::StaticClass(),
			UEdGraphSchema_K2::StaticClass()
		);

		if (!NewGraph) {
			return TResult<FString>::Failure(TEXT("Failed to create function graph"));
		}

		// Create function entry and result nodes
		// Note: AddFunctionGraph internally adds the graph to Blueprint->FunctionGraphs
		FBlueprintEditorUtils::AddFunctionGraph<UClass>(Blueprint, NewGraph, false, nullptr);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return TResult<FString>::Success(FunctionName);
	}

	auto FBlueprintMemberService::RemoveFunction(
		const FString& BlueprintName,
		const FString& FunctionName
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the function graph
		const auto FunctionFName = FName(*FunctionName);
		UEdGraph* FunctionGraph = nullptr;

		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (Graph->GetFName() == FunctionFName) {
				FunctionGraph = Graph;
				break;
			}
		}

		if (!FunctionGraph) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Function '%s' not found in blueprint"), *FunctionName)
			);
		}

		// Remove the function graph using Blueprint Editor Utils
		FBlueprintEditorUtils::RemoveGraph(Blueprint, FunctionGraph, EGraphRemoveFlags::Recompile);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::AddFunctionParameter(
		const FString& BlueprintName,
		const FString& FunctionName,
		const FString& ParamName,
		const FString& ParamType,
		bool bIsReference
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the function graph
		const auto FunctionFName = FName(*FunctionName);
		UEdGraph* FunctionGraph = nullptr;

		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (Graph->GetFName() == FunctionFName) {
				FunctionGraph = Graph;
				break;
			}
		}

		if (!FunctionGraph) {
			return FVoidResult::Failure(
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
			return FVoidResult::Failure(TEXT("Function entry node not found"));
		}

		// Create the pin type based on the parameter type
		FEdGraphPinType PinType;
		if (!FCommonUtils::ParsePinType(ParamType, PinType)) {
			return FVoidResult::Failure(
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

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::SetFunctionReturnType(
		const FString& BlueprintName,
		const FString& FunctionName,
		const FString& ReturnType
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the function graph
		const auto FunctionFName = FName(*FunctionName);
		UEdGraph* FunctionGraph = nullptr;

		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (Graph->GetFName() == FunctionFName) {
				FunctionGraph = Graph;
				break;
			}
		}

		if (!FunctionGraph) {
			return FVoidResult::Failure(
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
			}
			else {
				return FVoidResult::Failure(TEXT("Failed to create function result node"));
			}
		}

		// Create the pin type based on the return type
		FEdGraphPinType PinType;
		if (!FCommonUtils::ParsePinType(ReturnType, PinType)) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Unsupported return type: %s"), *ReturnType)
			);
		}

		// Add or update the return value pin
		const auto ReturnValueName = FName(TEXT("ReturnValue"));

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

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::SetFunctionMetadata(
		const FString& BlueprintName,
		const FString& FunctionName,
		const TOptional<FString>& Category,
		const TOptional<FString>& Tooltip,
		const TOptional<bool>& bPure
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the function graph
		const auto FunctionFName = FName(*FunctionName);
		UEdGraph* FunctionGraph = nullptr;

		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (Graph->GetFName() == FunctionFName) {
				FunctionGraph = Graph;
				break;
			}
		}

		if (!FunctionGraph) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Function '%s' not found in blueprint"), *FunctionName)
			);
		}

		// Compile the blueprint first to ensure the UFunction exists
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		// Get the compiled UFunction from the generated class
		UFunction* CompiledFunction = nullptr;
		if (Blueprint->GeneratedClass) {
			CompiledFunction = Blueprint->GeneratedClass->FindFunctionByName(FunctionFName);
		}

		if (!CompiledFunction) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Compiled function '%s' not found after blueprint compilation"), *FunctionName)
			);
		}

#if WITH_EDITORONLY_DATA
		// Set metadata on the compiled UFunction
		// Note: Metadata is editor-only and will not exist in shipping builds

		if (Category.IsSet()) {
			CompiledFunction->SetMetaData(FBlueprintMetadata::MD_FunctionCategory, *Category.GetValue());
		}

		if (Tooltip.IsSet()) {
			CompiledFunction->SetMetaData(FBlueprintMetadata::MD_Tooltip, *Tooltip.GetValue());
		}

		if (bPure.IsSet()) {
			if (bPure.GetValue()) {
				// Add pure flag
				CompiledFunction->FunctionFlags |= FUNC_BlueprintPure;
			}
			else {
				// Remove pure flag
				CompiledFunction->FunctionFlags &= ~FUNC_BlueprintPure;
			}
		}
#endif

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		return FVoidResult::Success();
	}

	// ============ Variable Operations ============

	auto FBlueprintMemberService::AddVariable(
		const FString& BlueprintName,
		const FString& VariableName,
		const FString& VariableType,
		bool bIsExposed
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Check if variable already exists
		const auto VarFName = FName(*VariableName);
		for (const FBPVariableDescription& ExistingVar : Blueprint->NewVariables) {
			if (ExistingVar.VarName == VarFName) {
				return FVoidResult::Failure(
					FString::Printf(TEXT("Variable '%s' already exists in blueprint"), *VariableName)
				);
			}
		}

		// Map string type to FEdGraphPinType
		FEdGraphPinType PinType;
		if (VariableType == TEXT("bool") || VariableType == TEXT("Boolean")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		}
		else if (VariableType == TEXT("int") || VariableType == TEXT("Integer")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
		}
		else if (VariableType == TEXT("float") || VariableType == TEXT("Float") || VariableType == TEXT("Real")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
		}
		else if (VariableType == TEXT("string") || VariableType == TEXT("String")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_String;
		}
		else if (VariableType == TEXT("name") || VariableType == TEXT("Name")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Name;
		}
		else if (VariableType == TEXT("vector") || VariableType == TEXT("Vector")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
		}
		else if (VariableType == TEXT("rotator") || VariableType == TEXT("Rotator")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
		}
		else if (VariableType == TEXT("transform") || VariableType == TEXT("Transform")) {
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = TBaseStructure<FTransform>::Get();
		}
		else {
			// Default to bool for unknown types
			PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		}

		// Add the member variable using Blueprint Editor Utils
		const bool bAdded = FBlueprintEditorUtils::AddMemberVariable(
			Blueprint,
			VarFName,
			PinType
		);

		if (!bAdded) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Failed to add variable '%s' to blueprint"), *VariableName)
			);
		}

		// Set exposure flag if requested
		if (bIsExposed) {
			// Get the property flags and set the instance editable flag
			if (uint64* PropertyFlags = FBlueprintEditorUtils::GetBlueprintVariablePropertyFlags(Blueprint, VarFName)) {
				*PropertyFlags &= ~CPF_DisableEditOnInstance; // Clear the disable edit flag
			}
		}

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::RemoveVariable(
		const FString& BlueprintName,
		const FString& VariableName
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the variable
		const auto VarFName = FName(*VariableName);
		const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VarFName);

		if (VarIndex == INDEX_NONE) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Variable '%s' not found in blueprint"), *VariableName)
			);
		}

		// Remove the variable using Blueprint Editor Utils
		FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarFName);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::RenameVariable(
		const FString& BlueprintName,
		const FString& OldName,
		const FString& NewName
	) -> FVoidResult {
		if (NewName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("New variable name cannot be empty"));
		}

		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the old variable
		const auto OldVarFName = FName(*OldName);
		const int32 OldVarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, OldVarFName);

		if (OldVarIndex == INDEX_NONE) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Variable '%s' not found in blueprint"), *OldName)
			);
		}

		// Check if new name already exists
		const auto NewVarFName = FName(*NewName);
		const int32 NewVarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, NewVarFName);

		if (NewVarIndex != INDEX_NONE) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Variable with name '%s' already exists"), *NewName)
			);
		}

		// Rename the variable using Blueprint Editor Utils
		FBlueprintEditorUtils::RenameMemberVariable(Blueprint, OldVarFName, NewVarFName);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::SetVariableDefaultValue(
		const FString& BlueprintName,
		const FString& VariableName,
		const TSharedPtr<FJsonValue>& Value
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the variable
		const auto VarFName = FName(*VariableName);
		const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VarFName);

		if (VarIndex == INDEX_NONE) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Variable '%s' not found in blueprint"), *VariableName)
			);
		}

		// Get the variable type
		FBPVariableDescription& Variable = Blueprint->NewVariables[VarIndex];
		FString DefaultValueStr;

		// Convert JSON value to string based on variable type
		if (Variable.VarType.PinCategory == UEdGraphSchema_K2::PC_Boolean) {
			bool bValue;
			if (!Value->TryGetBool(bValue)) {
				return FVoidResult::Failure(TEXT("Value is not a boolean"));
			}
			DefaultValueStr = bValue ? TEXT("true") : TEXT("false");
		}
		else if (Variable.VarType.PinCategory == UEdGraphSchema_K2::PC_Int) {
			double NumValue;
			if (!Value->TryGetNumber(NumValue)) {
				return FVoidResult::Failure(TEXT("Value is not a number"));
			}
			DefaultValueStr = FString::FromInt(static_cast<int32>(NumValue));
		}
		else if (Variable.VarType.PinCategory == UEdGraphSchema_K2::PC_Real) {
			double NumValue;
			if (!Value->TryGetNumber(NumValue)) {
				return FVoidResult::Failure(TEXT("Value is not a number"));
			}
			DefaultValueStr = FString::SanitizeFloat(NumValue);
		}
		else if (Variable.VarType.PinCategory == UEdGraphSchema_K2::PC_String) {
			FString StrValue;
			if (!Value->TryGetString(StrValue)) {
				return FVoidResult::Failure(TEXT("Value is not a string"));
			}
			DefaultValueStr = StrValue;
		}
		else if (Variable.VarType.PinCategory == UEdGraphSchema_K2::PC_Name) {
			FString StrValue;
			if (!Value->TryGetString(StrValue)) {
				return FVoidResult::Failure(TEXT("Value is not a string"));
			}
			DefaultValueStr = StrValue;
		}
		else if (Variable.VarType.PinCategory == UEdGraphSchema_K2::PC_Struct) {
			// Handle struct types (Vector, Rotator, Transform)
			const TSharedPtr<FJsonObject>* ValueObj;
			if (!Value->TryGetObject(ValueObj)) {
				return FVoidResult::Failure(TEXT("Value is not an object"));
			}

			if (Variable.VarType.PinSubCategoryObject == TBaseStructure<FVector>::Get()) {
				const double X = (*ValueObj)->GetNumberField(TEXT("x"));
				const double Y = (*ValueObj)->GetNumberField(TEXT("y"));
				const double Z = (*ValueObj)->GetNumberField(TEXT("z"));
				DefaultValueStr = FString::Printf(TEXT("%f,%f,%f"), X, Y, Z);
			}
			else if (Variable.VarType.PinSubCategoryObject == TBaseStructure<FRotator>::Get()) {
				const double Pitch = (*ValueObj)->GetNumberField(TEXT("pitch"));
				const double Yaw = (*ValueObj)->GetNumberField(TEXT("yaw"));
				const double Roll = (*ValueObj)->GetNumberField(TEXT("roll"));
				DefaultValueStr = FString::Printf(TEXT("%f,%f,%f"), Pitch, Yaw, Roll);
			}
			else {
				return FVoidResult::Failure(TEXT("Unsupported struct type"));
			}
		}
		else {
			return FVoidResult::Failure(TEXT("Unsupported variable type for default value"));
		}

		// Set the default value
		Variable.DefaultValue = DefaultValueStr;

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::SetVariableMetadata(
		const FString& BlueprintName,
		const FString& VariableName,
		const TOptional<FString>& Tooltip,
		const TOptional<FString>& Category,
		const TOptional<bool>& bExposeOnSpawn,
		const TOptional<bool>& bInstanceEditable,
		const TOptional<bool>& bBlueprintReadOnly
	) -> FVoidResult {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the variable
		const auto VarFName = FName(*VariableName);
		const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VarFName);

		if (VarIndex == INDEX_NONE) {
			return FVoidResult::Failure(
				FString::Printf(TEXT("Variable '%s' not found in blueprint"), *VariableName)
			);
		}

		// Get the variable
		FBPVariableDescription& Variable = Blueprint->NewVariables[VarIndex];

		// Set optional metadata fields
		if (Tooltip.IsSet()) {
			FBlueprintEditorUtils::SetBlueprintVariableMetaData(
				Blueprint,
				VarFName,
				nullptr,
				FBlueprintMetadata::MD_Tooltip,
				Tooltip.GetValue()
			);
		}

		if (Category.IsSet()) {
			FBlueprintEditorUtils::SetBlueprintVariableCategory(
				Blueprint,
				VarFName,
				nullptr,
				FText::FromString(Category.GetValue())
			);
		}

		if (bExposeOnSpawn.IsSet()) {
			Variable.PropertyFlags = bExposeOnSpawn.GetValue()
				? (Variable.PropertyFlags | CPF_ExposeOnSpawn)
				: (Variable.PropertyFlags & ~CPF_ExposeOnSpawn);
		}

		if (bInstanceEditable.IsSet()) {
			Variable.PropertyFlags = bInstanceEditable.GetValue()
				? (Variable.PropertyFlags & ~CPF_DisableEditOnInstance)
				: (Variable.PropertyFlags | CPF_DisableEditOnInstance);
		}

		if (bBlueprintReadOnly.IsSet()) {
			Variable.PropertyFlags = bBlueprintReadOnly.GetValue()
				? (Variable.PropertyFlags | CPF_BlueprintReadOnly)
				: (Variable.PropertyFlags & ~CPF_BlueprintReadOnly);
		}

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FVoidResult::Success();
	}

	auto FBlueprintMemberService::GetFunctions(const FString& BlueprintName) -> TResult<FGetBlueprintFunctionsResult> {
		// Find the blueprint
		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint) {
			return TResult<FGetBlueprintFunctionsResult>::Failure(
				FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName)
			);
		}

		FGetBlueprintFunctionsResult Result;
		Result.Functions.Reserve(Blueprint->FunctionGraphs.Num());

		// Ensure the blueprint is compiled to have access to UFunction metadata
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (!Graph) {
				continue;
			}

			FBlueprintFunctionInfo FunctionInfo;
			FunctionInfo.Name = Graph->GetName();
			FunctionInfo.NodeCount = Graph->Nodes.Num();

			// Find the function entry node
			UK2Node_FunctionEntry* EntryNode = nullptr;
			for (UEdGraphNode* Node : Graph->Nodes) {
				if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(Node)) {
					EntryNode = Entry;
					break;
				}
			}

			if (EntryNode) {
				// Extract parameters (output pins excluding execution pin)
				for (const UEdGraphPin* Pin : EntryNode->Pins) {
					if (Pin && Pin->Direction == EGPD_Output && Pin->PinName != UEdGraphSchema_K2::PN_Then) {
						FBlueprintFunctionParam Param;
						Param.Name = Pin->PinName.ToString();
						Param.Type = Pin->PinType.PinCategory.ToString();
						Param.bIsArray = Pin->PinType.IsArray();
						Param.bIsReference = Pin->PinType.bIsReference;

						if (Pin->PinType.PinSubCategoryObject.IsValid()) {
							Param.SubType = Pin->PinType.PinSubCategoryObject->GetName();
						}

						FunctionInfo.Parameters.Add(Param);
					}
				}

				// Extract return values (input pins excluding execution pin)
				for (const UEdGraphPin* Pin : EntryNode->Pins) {
					if (Pin && Pin->Direction == EGPD_Input && Pin->PinName != UEdGraphSchema_K2::PN_Execute) {
						FBlueprintFunctionParam ReturnParam;
						ReturnParam.Name = Pin->PinName.ToString();
						ReturnParam.Type = Pin->PinType.PinCategory.ToString();
						ReturnParam.bIsArray = Pin->PinType.IsArray();
						ReturnParam.bIsReference = false; // Return values are typically not references

						if (Pin->PinType.PinSubCategoryObject.IsValid()) {
							ReturnParam.SubType = Pin->PinType.PinSubCategoryObject->GetName();
						}

						FunctionInfo.Returns.Add(ReturnParam);
					}
				}
			}

			// Extract metadata from the compiled UFunction
			if (Blueprint->GeneratedClass) {
				const auto FunctionFName = FName(*Graph->GetName());
				UFunction* CompiledFunction = Blueprint->GeneratedClass->FindFunctionByName(FunctionFName);

				if (CompiledFunction) {
					FunctionInfo.Category = CompiledFunction->GetMetaData(FBlueprintMetadata::MD_FunctionCategory);

					FunctionInfo.Tooltip = CompiledFunction->GetMetaData(FBlueprintMetadata::MD_Tooltip);

					FunctionInfo.Keywords = CompiledFunction->GetMetaData(TEXT("Keywords"));

					FunctionInfo.bIsPure = (CompiledFunction->FunctionFlags & FUNC_BlueprintPure) != 0;
				}
				else {
					// Fallback values if compiled function is not found
					FunctionInfo.Category = TEXT("Default");
					FunctionInfo.Tooltip = TEXT("");
					FunctionInfo.Keywords = TEXT("");
					FunctionInfo.bIsPure = false;
				}
			}
			else {
				// Fallback for non-compiled blueprints
				FunctionInfo.Category = TEXT("Default");
				FunctionInfo.Tooltip = TEXT("");
				FunctionInfo.Keywords = TEXT("");
				FunctionInfo.bIsPure = false;
			}

			Result.Functions.Add(FunctionInfo);
		}

		Result.Count = Result.Functions.Num();
		return TResult<FGetBlueprintFunctionsResult>::Success(Result);
	}

}
