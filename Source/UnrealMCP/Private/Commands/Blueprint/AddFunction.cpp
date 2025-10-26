#include "Commands/Blueprint/AddFunction.h"
#include "Core/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FAddFunctionCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse required parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name and function_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));

		// Find the blueprint
		UBlueprint* Blueprint =
			LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName));
		if (!Blueprint) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Check if function already exists
		const FName FunctionFName = FName(*FunctionName);
		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (Graph->GetFName() == FunctionFName) {
				return FCommonUtils::CreateErrorResponse(
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
			return FCommonUtils::CreateErrorResponse(TEXT("Failed to create function graph"));
		}

		// Add the graph to the blueprint's function graphs
		Blueprint->FunctionGraphs.Add(NewGraph);

		// Create function entry and result nodes
		FBlueprintEditorUtils::AddFunctionGraph<UClass>(Blueprint, NewGraph, false, nullptr);

		// Mark the blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Function '%s' added to blueprint '%s'"), *FunctionName, *BlueprintName)
			);
			Data->SetStringField(TEXT("function_name"), FunctionName);
		});
	}

} // namespace UnrealMCP
