#include "Commands/Blueprint/SetPhysicsProperties.h"

#include "Commands/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

auto FSetPhysicsProperties::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FPhysicsParams> ParamsResult =
		UnrealMCP::FPhysicsParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FBlueprintService::SetPhysicsProperties(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("component"), ParamsResult.GetValue().ComponentName);
	Response->SetBoolField(TEXT("success"), true);
	return Response;
}
