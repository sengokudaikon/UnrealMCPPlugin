#include "Commands/BlueprintNode/FindBlueprintNodes.h"
#include "Commands/CommonUtils.h"
#include "Services/BlueprintGraphService.h"

auto FFindBlueprintNodes::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
	}

	FString NodeType;
	if (!Params->TryGetStringField(TEXT("node_type"), NodeType)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'node_type' parameter"));
	}

	TOptional<FString> EventName;
	FString EventNameStr;
	if (Params->TryGetStringField(TEXT("event_name"), EventNameStr)) {
		EventName = EventNameStr;
	}

	TArray<FString> NodeGuids;
	const UnrealMCP::FVoidResult Result = UnrealMCP::FBlueprintGraphService::FindNodes(
		BlueprintName,
		NodeType,
		EventName,
		NodeGuids
	);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	TArray<TSharedPtr<FJsonValue>> NodeGuidArray;
	for (const FString& Guid : NodeGuids) {
		NodeGuidArray.Add(MakeShared<FJsonValueString>(Guid));
	}

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetArrayField(TEXT("node_guids"), NodeGuidArray);
	return Response;
}
