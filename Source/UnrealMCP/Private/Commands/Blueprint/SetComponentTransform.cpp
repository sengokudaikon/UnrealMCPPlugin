#include "Commands/Blueprint/SetComponentTransform.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintService.h"

namespace UnrealMCP {

	auto FSetComponentTransformCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		const auto TransformParams = FComponentTransformParams::FromJson(Params);
		if (!TransformParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(TransformParams.GetError());
		}

		const auto Result = FBlueprintService::SetComponentTransform(TransformParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Transform updated for component '%s'"), *TransformParams.GetValue().ComponentName)
			);
			Data->SetObjectField(TEXT("transform"), Result.GetValue().ToJson());
		});
	}

}
