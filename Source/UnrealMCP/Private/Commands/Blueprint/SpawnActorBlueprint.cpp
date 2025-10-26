#include "Commands/Blueprint/SpawnActorBlueprint.h"

#include "Commands/CommonUtils.h"
#include "Services/BlueprintService.h"
#include "Core/MCPTypes.h"

auto FSpawnActorBlueprint::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FBlueprintSpawnParams> ParamsResult =
		UnrealMCP::FBlueprintSpawnParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	UnrealMCP::TResult<AActor*> Result =
		UnrealMCP::FBlueprintService::SpawnActorBlueprint(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	return FCommonUtils::ActorToJsonObject(Result.GetValue(), true);
}
