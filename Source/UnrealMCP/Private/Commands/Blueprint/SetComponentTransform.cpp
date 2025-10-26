#include "Commands/Blueprint/SetComponentTransform.h"
#include "Services/BlueprintService.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	auto FSetComponentTransformCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse and validate parameters using the service
		const auto TransformParams = FComponentTransformParams::FromJson(Params);
		if (!TransformParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(TransformParams.GetError());
		}

		// Delegate to service layer
		const auto Result = FBlueprintService::SetComponentTransform(TransformParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		// Format response
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Transform updated for component '%s'"), *TransformParams.GetValue().ComponentName)
			);
			Data->SetObjectField(TEXT("transform"), Result.GetValue().ToJson());
		});
	}

} // namespace UnrealMCP
