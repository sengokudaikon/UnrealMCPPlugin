#include "Commands/Editor/SetActorProperty.h"
#include "Core/CommonUtils.h"
#include "Services/ActorService.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"

auto FSetActorProperty::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString ActorName;
	if (!Params->TryGetStringField(TEXT("name"), ActorName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
	}

	FString PropertyName;
	if (!Params->TryGetStringField(TEXT("property_name"), PropertyName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'property_name' parameter"));
	}

	if (!Params->HasField(TEXT("property_value"))) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'property_value' parameter"));
	}

	const TSharedPtr<FJsonValue> PropertyValue = Params->Values.FindRef(TEXT("property_value"));

	const UnrealMCP::FVoidResult Result = UnrealMCP::FActorService::SetActorProperty(
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