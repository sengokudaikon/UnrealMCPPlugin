#include "Commands/Blueprint/GetBlueprintFunctions.h"
#include "EdGraph/EdGraph.h"
#include "Engine/Blueprint.h"
#include "K2Node_FunctionEntry.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	auto FGetBlueprintFunctionsCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse parameters
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing required parameter: blueprint_name"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));

		// Find the blueprint
		UBlueprint* Blueprint =
			LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName));
		if (!Blueprint) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Collect function information
		TArray<TSharedPtr<FJsonValue>> FunctionsArray;

		// Iterate through all function graphs
		for (UEdGraph* Graph : Blueprint->FunctionGraphs) {
			if (!Graph)
				continue;

			auto FunctionObj = MakeShared<FJsonObject>();
			FunctionObj->SetStringField(TEXT("name"), Graph->GetName());

			// Find function entry node to get parameters
			UK2Node_FunctionEntry* EntryNode = nullptr;
			for (UEdGraphNode* Node : Graph->Nodes) {
				if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(Node)) {
					EntryNode = Entry;
					break;
				}
			}

			if (EntryNode) {
				// Get parameters
				TArray<TSharedPtr<FJsonValue>> ParamsArray;
				for (const UEdGraphPin* Pin : EntryNode->Pins) {
					if (Pin && Pin->Direction == EGPD_Output && Pin->PinName != UEdGraphSchema_K2::PN_Then) {
						auto ParamObj = MakeShared<FJsonObject>();
						ParamObj->SetStringField(TEXT("name"), Pin->PinName.ToString());
						ParamObj->SetStringField(TEXT("type"), Pin->PinType.PinCategory.ToString());

						// Add subcategory if it exists (for object types)
						if (Pin->PinType.PinSubCategoryObject.IsValid()) {
							ParamObj->SetStringField(TEXT("subtype"), Pin->PinType.PinSubCategoryObject->GetName());
						}

						ParamObj->SetBoolField(TEXT("is_array"), Pin->PinType.IsArray());
						ParamObj->SetBoolField(TEXT("is_reference"), Pin->PinType.bIsReference);

						ParamsArray.Add(MakeShared<FJsonValueObject>(ParamObj));
					}
				}
				FunctionObj->SetArrayField(TEXT("parameters"), ParamsArray);

				// Get return value
				TArray<TSharedPtr<FJsonValue>> ReturnsArray;
				for (const UEdGraphPin* Pin : EntryNode->Pins) {
					if (Pin && Pin->Direction == EGPD_Input && Pin->PinName != UEdGraphSchema_K2::PN_Execute) {
						auto ReturnObj = MakeShared<FJsonObject>();
						ReturnObj->SetStringField(TEXT("name"), Pin->PinName.ToString());
						ReturnObj->SetStringField(TEXT("type"), Pin->PinType.PinCategory.ToString());

						if (Pin->PinType.PinSubCategoryObject.IsValid()) {
							ReturnObj->SetStringField(TEXT("subtype"), Pin->PinType.PinSubCategoryObject->GetName());
						}

						ReturnObj->SetBoolField(TEXT("is_array"), Pin->PinType.IsArray());

						ReturnsArray.Add(MakeShared<FJsonValueObject>(ReturnObj));
					}
				}
				FunctionObj->SetArrayField(TEXT("returns"), ReturnsArray);

				// Get function metadata - simplified for now
				FunctionObj->SetStringField(TEXT("category"), TEXT("Default"));
				FunctionObj->SetStringField(TEXT("keywords"), TEXT(""));
				FunctionObj->SetStringField(TEXT("tooltip"), TEXT("Function tooltip"));
				FunctionObj->SetBoolField(TEXT("is_pure"), false);
			}

			// Add node count
			FunctionObj->SetNumberField(TEXT("node_count"), Graph->Nodes.Num());

			FunctionsArray.Add(MakeShared<FJsonValueObject>(FunctionObj));
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetArrayField(TEXT("functions"), FunctionsArray);
			Data->SetNumberField(TEXT("count"), FunctionsArray.Num());
		});
	}

} // namespace UnrealMCP
