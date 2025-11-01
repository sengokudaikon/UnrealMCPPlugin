#include "Commands/BlueprintNode/AddBlueprintSelfReference.h"
#include "K2Node_Self.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Services/BlueprintGraphService.h"

namespace UnrealMCP {
	auto FAddBlueprintSelfReference::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString BlueprintName;
		if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter")));
		}

		FVector2D NodePosition(0.0f, 0.0f);
		if (Params->HasField(TEXT("node_position"))) {
			NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));
		}

		const TResult<UK2Node_Self*> Result = FBlueprintGraphService::AddSelfReferenceNode(
			BlueprintName,
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
