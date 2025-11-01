#include "Commands/Editor/SetActorProperty.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"
#include "Services/ActorService.h"

namespace UnrealMCP {

	auto FSetActorProperty::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString ActorName;
		if (!Params->TryGetStringField(TEXT("name"), ActorName)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'name' parameter")));
		}

		FString PropertyName;
		if (!Params->TryGetStringField(TEXT("property_name"), PropertyName)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'property_name' parameter")));
		}

		if (!Params->HasField(TEXT("property_value"))) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'property_value' parameter")));
		}

		const TSharedPtr<FJsonValue> PropertyValue = Params->Values.FindRef(TEXT("property_value"));

		const FVoidResult Result = FActorService::SetActorProperty(
			ActorName,
			PropertyName,
			PropertyValue
		);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("actor"), ActorName);
			Data->SetStringField(TEXT("property"), PropertyName);
			Data->SetBoolField(TEXT("success"), true);
		});
	}
}
