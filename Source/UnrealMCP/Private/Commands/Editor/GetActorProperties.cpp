#include "Commands/Editor/GetActorProperties.h"
#include "Commands/CommonUtils.h"
#include "Services/ActorService.h"
#include "Core/MCPTypes.h"
#include "GameFramework/Actor.h"

auto FGetActorProperties::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString ActorName;
	if (!Params->TryGetStringField(TEXT("name"), ActorName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
	}

	TMap<FString, FString> Properties;
	const UnrealMCP::FVoidResult Result = UnrealMCP::FActorService::GetActorProperties(ActorName, Properties);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const TSharedPtr<FJsonObject> PropertiesObj = MakeShared<FJsonObject>();
	for (const auto& Property : Properties) {
		PropertiesObj->SetStringField(Property.Key, Property.Value);
	}

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("actor"), ActorName);
	Response->SetObjectField(TEXT("properties"), PropertiesObj);
	return Response;
}