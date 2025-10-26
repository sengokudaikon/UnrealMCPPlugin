#include "Commands/Editor/GetActorProperties.h"
#include "Core/CommonUtils.h"
#include "Services/ActorService.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"

namespace UnrealMCP {

auto FGetActorProperties::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString ActorName;
	if (!Params->TryGetStringField(TEXT("name"), ActorName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
	}

	TMap<FString, FString> Properties;

	if (const FVoidResult Result = FActorService::GetActorProperties(ActorName, Properties); Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const TSharedPtr<FJsonObject> PropertiesObj = MakeShared<FJsonObject>();
	for (const auto& Property : Properties) {
		PropertiesObj->SetStringField(Property.Key, Property.Value);
	}

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("actor"), ActorName);
		Data->SetObjectField(TEXT("properties"), PropertiesObj);
	});
}}