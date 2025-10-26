#include "Commands/Editor/GetActorsInLevel.h"
#include "Commands/CommonUtils.h"
#include "Services/ActorService.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"

auto FGetActorsInLevel::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	TArray<FString> ActorNames;
	const UnrealMCP::FVoidResult Result = UnrealMCP::FActorService::GetActorsInLevel(ActorNames);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	TArray<TSharedPtr<FJsonValue>> ActorArray;
	for (const FString& ActorName : ActorNames) {
		TSharedPtr<FJsonObject> ActorObj = MakeShared<FJsonObject>();
		ActorObj->SetStringField(TEXT("name"), ActorName);
		ActorArray.Add(MakeShared<FJsonValueObject>(ActorObj));
	}

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetArrayField(TEXT("actors"), ActorArray);
	return Response;
}