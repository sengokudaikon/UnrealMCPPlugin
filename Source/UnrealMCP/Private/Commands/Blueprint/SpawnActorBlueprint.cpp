#include "Commands/Blueprint/SpawnActorBlueprint.h"

#include "Core/CommonUtils.h"
#include "Services/BlueprintService.h"
#include "Core/MCPTypes.h"

namespace UnrealMCP {

auto FSpawnActorBlueprint::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	TResult<FBlueprintSpawnParams> ParamsResult =
		FBlueprintSpawnParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	TResult<AActor*> Result =
		FBlueprintService::SpawnActorBlueprint(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	return FCommonUtils::ActorToJsonObject(Result.GetValue(), true);
}
}
