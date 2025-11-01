#include "Commands/Editor/SetActorTransform.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"
#include "Services/ActorService.h"

namespace UnrealMCP {

	auto FSetActorTransform::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString ActorName;
		if (!Params->TryGetStringField(TEXT("name"), ActorName)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'name' parameter")));
		}

		TOptional<FVector> Location;
		if (Params->HasField(TEXT("location"))) {
			Location = FCommonUtils::GetVectorFromJson(Params, TEXT("location"));
		}

		TOptional<FRotator> Rotation;
		if (Params->HasField(TEXT("rotation"))) {
			Rotation = FCommonUtils::GetRotatorFromJson(Params, TEXT("rotation"));
		}

		TOptional<FVector> Scale;
		if (Params->HasField(TEXT("scale"))) {
			Scale = FCommonUtils::GetVectorFromJson(Params, TEXT("scale"));
		}

		const FVoidResult Result = FActorService::SetActorTransform(
			ActorName,
			Location,
			Rotation,
			Scale
		);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("actor"), ActorName);
			Data->SetBoolField(TEXT("success"), true);

			if (Location.IsSet()) {
				const FVector Loc = Location.GetValue();
				const TSharedPtr<FJsonObject> LocationObj = MakeShared<FJsonObject>();
				LocationObj->SetNumberField(TEXT("x"), Loc.X);
				LocationObj->SetNumberField(TEXT("y"), Loc.Y);
				LocationObj->SetNumberField(TEXT("z"), Loc.Z);
				Data->SetObjectField(TEXT("location"), LocationObj);
			}
		});
	}
}
