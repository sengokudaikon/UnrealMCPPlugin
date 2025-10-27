#include "Commands/BlueprintNode/AddBlueprintGetSelfComponentReference.h"
#include "K2Node_VariableGet.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintGraphService.h"

namespace UnrealMCP {
	auto FAddBlueprintGetSelfComponentReference::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		FString BlueprintName;
		if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
		}

		FString ComponentName;
		if (!Params->TryGetStringField(TEXT("component_name"), ComponentName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'component_name' parameter"));
		}

		FVector2D NodePosition(0.0f, 0.0f);
		if (Params->HasField(TEXT("node_position"))) {
			NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));
		}

		TResult<UK2Node_VariableGet*> Result = FBlueprintGraphService::AddComponentReferenceNode(
			BlueprintName,
			ComponentName,
			NodePosition
		);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("node_id"), Result.GetValue()->NodeGuid.ToString());
		});
	}
}
