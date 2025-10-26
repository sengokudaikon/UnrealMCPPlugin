#include "Commands/Editor/GetActorsInLevel.h"
#include "Core/CommonUtils.h"
#include "Services/ActorService.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"

namespace UnrealMCP {

auto FGetActorsInLevel::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	TArray<FString> ActorNames;

	if (const FVoidResult Result = FActorService::GetActorsInLevel(ActorNames); Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	TArray<TSharedPtr<FJsonValue>> ActorArray;
	for (const FString& ActorName : ActorNames) {
		TSharedPtr<FJsonObject> ActorObj = MakeShared<FJsonObject>();
		ActorObj->SetStringField(TEXT("name"), ActorName);
		ActorArray.Add(MakeShared<FJsonValueObject>(ActorObj));
	}

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetArrayField(TEXT("actors"), ActorArray);
	});
}}