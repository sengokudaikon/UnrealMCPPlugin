#include "Commands/BlueprintNode/ConnectBlueprintNodes.h"
#include "Commands/CommonUtils.h"
#include "Services/BlueprintGraphService.h"

auto FConnectBlueprintNodes::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
	}

	FString SourceNodeId;
	if (!Params->TryGetStringField(TEXT("source_node_id"), SourceNodeId)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'source_node_id' parameter"));
	}

	FString TargetNodeId;
	if (!Params->TryGetStringField(TEXT("target_node_id"), TargetNodeId)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'target_node_id' parameter"));
	}

	FString SourcePinName;
	if (!Params->TryGetStringField(TEXT("source_pin"), SourcePinName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'source_pin' parameter"));
	}

	FString TargetPinName;
	if (!Params->TryGetStringField(TEXT("target_pin"), TargetPinName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'target_pin' parameter"));
	}

	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::ConnectNodes(
		BlueprintName,
		SourceNodeId,
		TargetNodeId,
		SourcePinName,
		TargetPinName
	);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("source_node_id"), SourceNodeId);
	Response->SetStringField(TEXT("target_node_id"), TargetNodeId);
	return Response;
}
