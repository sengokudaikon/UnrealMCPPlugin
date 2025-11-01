#include "Commands/Editor/GetActorAvailableProperties.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"
#include "Services/ActorService.h"

namespace UnrealMCP {

	auto FGetActorAvailableProperties::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString ActorName;
		if (!Params->TryGetStringField(TEXT("name"), ActorName)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'name' parameter")));
		}

		AActor* Actor = FActorService::FindActorByName(ActorName);
		if (!Actor) {
			TArray<FString> AvailableActors;
			FActorService::GetActorsInLevel(AvailableActors);

			FString ErrorMsg = FString::Printf(TEXT("Actor not found: %s. "), *ActorName);

			if (AvailableActors.Num() > 0) {
				ErrorMsg += TEXT("Available actors: ");
				for (int32 i = 0; i < FMath::Min(5, AvailableActors.Num()); i++) {
					ErrorMsg += AvailableActors[i];
					if (i < FMath::Min(5, AvailableActors.Num()) - 1) {
						ErrorMsg += TEXT(", ");
					}
				}
				if (AvailableActors.Num() > 5) {
					ErrorMsg += TEXT("...");
				}
			} else {
				ErrorMsg += TEXT("No actors found in the current level.");
			}

			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::ActorNotFound, ErrorMsg));
		}

		TArray<FString> AvailableProperties = FActorService::GetAvailableProperties(Actor->GetClass());

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("actor"), ActorName);
			Data->SetStringField(TEXT("class"), Actor->GetClass()->GetName());

			TSharedPtr<FJsonObject> PropertiesArray = MakeShareable(new FJsonObject());
			TArray<TSharedPtr<FJsonValue>> PropertiesJson;

			for (const FString& Property : AvailableProperties) {
				PropertiesJson.Add(MakeShareable(new FJsonValueString(Property)));
			}

			PropertiesArray->SetArrayField(TEXT("available_properties"), PropertiesJson);
			Data->SetObjectField(TEXT("properties"), PropertiesArray);

			Data->SetNumberField(TEXT("count"), AvailableProperties.Num());
		});
	}
}