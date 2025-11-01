#include "Commands/Editor/FindActorsByName.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"
#include "Services/ActorService.h"

namespace UnrealMCP {

	auto FFindActorsByName::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString Pattern;
		if (!Params->TryGetStringField(TEXT("pattern"), Pattern)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'pattern' parameter")));
		}

		TArray<FString> ActorNames;

		if (const FVoidResult Result = FActorService::FindActorsByName(Pattern, ActorNames);
			Result.IsFailure()) {
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
	}
}
