#include "Commands/Editor/DeleteActor.h"
#include "Core/CommonUtils.h"
#include "Services/ActorService.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"

auto FDeleteActor::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString ActorName;
	if (!Params->TryGetStringField(TEXT("name"), ActorName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
	}

	if (const UnrealMCP::FVoidResult Result = UnrealMCP::FActorService::DeleteActor(ActorName); Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("deleted_actor"), ActorName);
		Data->SetBoolField(TEXT("success"), true);
	});
}
