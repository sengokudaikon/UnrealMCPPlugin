#include "Commands/BlueprintNode/AddBlueprintFunctionCall.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintGraphService.h"
#include "K2Node_CallFunction.h"

namespace UnrealMCP {
	auto FAddBlueprintFunctionCall::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString BlueprintName;
		if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
		}

		FString FunctionName;
		if (!Params->TryGetStringField(TEXT("function_name"), FunctionName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'function_name' parameter"));
		}

		FVector2D NodePosition(0.0f, 0.0f);
		if (Params->HasField(TEXT("node_position"))) {
			NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));
		}

		TOptional<FString> Target;
		FString TargetStr;
		if (Params->TryGetStringField(TEXT("target"), TargetStr)) {
			Target = TargetStr;
		}

		TSharedPtr<FJsonObject> Parameters;
		if (const TSharedPtr<FJsonObject>* ParamsObj; Params->TryGetObjectField(TEXT("params"), ParamsObj)) {
			Parameters = *ParamsObj;
		}

		const TResult<UK2Node_CallFunction*> Result = FBlueprintGraphService::AddFunctionCallNode(
			BlueprintName,
			FunctionName,
			Target,
			NodePosition,
			Parameters
		);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("node_id"), Result.GetValue()->NodeGuid.ToString());
		});
	}
}
