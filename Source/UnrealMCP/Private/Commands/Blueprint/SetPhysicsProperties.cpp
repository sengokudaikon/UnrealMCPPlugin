#include "Commands/Blueprint/SetPhysicsProperties.h"

#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

namespace UnrealMCP {

	auto FSetPhysicsProperties::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		TResult<FPhysicsParams> ParamsResult =
			FPhysicsParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const FVoidResult Result =
			FBlueprintService::SetPhysicsProperties(ParamsResult.GetValue());

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("component"), ParamsResult.GetValue().ComponentName);
		});
	}

}
