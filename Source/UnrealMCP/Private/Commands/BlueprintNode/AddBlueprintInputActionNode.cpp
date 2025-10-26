#include "Commands/BlueprintNode/AddBlueprintInputActionNode.h"
#include "Commands/CommonUtils.h"
#include "Services/BlueprintGraphService.h"
#include "K2Node_InputAction.h"

auto FAddBlueprintInputActionNode::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
	}

	FString ActionName;
	if (!Params->TryGetStringField(TEXT("action_name"), ActionName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'action_name' parameter"));
	}

	FVector2D NodePosition(0.0f, 0.0f);
	if (Params->HasField(TEXT("node_position"))) {
		NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));
	}

	const UnrealMCP::TResult<UK2Node_InputAction*> Result = UnrealMCP::FBlueprintGraphService::AddInputActionNode(
		BlueprintName,
		ActionName,
		NodePosition
	);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("node_id"), Result.GetValue()->NodeGuid.ToString());
	return Response;
}
