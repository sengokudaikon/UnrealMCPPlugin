#include "Commands/BlueprintNode/AddBlueprintEvent.h"
#include "K2Node_Event.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintGraphService.h"

namespace UnrealMCP {
	auto FAddBlueprintEvent::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString BlueprintName;
		if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
		}

		FString EventName;
		if (!Params->TryGetStringField(TEXT("event_name"), EventName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'event_name' parameter"));
		}

		FVector2D NodePosition(0.0f, 0.0f);
		if (Params->HasField(TEXT("node_position"))) {
			NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));
		}

		const TResult<UK2Node_Event*> Result = FBlueprintGraphService::AddEventNode(
			BlueprintName,
			EventName,
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
