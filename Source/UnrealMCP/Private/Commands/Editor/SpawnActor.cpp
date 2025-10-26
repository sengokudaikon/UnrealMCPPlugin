#include "Commands/Editor/SpawnActor.h"
#include "Core/CommonUtils.h"
#include "Services/ActorService.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"

namespace UnrealMCP {

auto FSpawnActor::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString ActorClass;
	if (!Params->TryGetStringField(TEXT("actor_class"), ActorClass)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'actor_class' parameter"));
	}

	FString ActorName;
	if (!Params->TryGetStringField(TEXT("actor_name"), ActorName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'actor_name' parameter"));
	}

	TOptional<FVector> Location;
	if (Params->HasField(TEXT("location"))) {
		Location = FCommonUtils::GetVectorFromJson(Params, TEXT("location"));
	}

	TOptional<FRotator> Rotation;
	if (Params->HasField(TEXT("rotation"))) {
		Rotation = FCommonUtils::GetRotatorFromJson(Params, TEXT("rotation"));
	}

	const TResult<AActor*> Result = FActorService::SpawnActor(
		ActorClass,
		ActorName,
		Location,
		Rotation
	);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const AActor* SpawnedActor = Result.GetValue();
	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("actor_name"), SpawnedActor->GetName());
		Data->SetStringField(TEXT("actor_class"), SpawnedActor->GetClass()->GetName());

		const FVector ActorLocation = SpawnedActor->GetActorLocation();
		const TSharedPtr<FJsonObject> LocationObj = MakeShared<FJsonObject>();
		LocationObj->SetNumberField(TEXT("x"), ActorLocation.X);
		LocationObj->SetNumberField(TEXT("y"), ActorLocation.Y);
		LocationObj->SetNumberField(TEXT("z"), ActorLocation.Z);
		Data->SetObjectField(TEXT("location"), LocationObj);
	});
}}
