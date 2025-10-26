#include "Commands/BlueprintNode/AddBlueprintGetSelfComponentReference.h"
#include "Commands/CommonUtils.h"
#include "Services/BlueprintGraphService.h"
#include "K2Node_VariableGet.h"

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

	UnrealMCP::TResult<UK2Node_VariableGet*> Result = UnrealMCP::FBlueprintGraphService::AddComponentReferenceNode(
		BlueprintName,
		ComponentName,
		NodePosition
	);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("node_id"), Result.GetValue()->NodeGuid.ToString());
	return Response;
}
