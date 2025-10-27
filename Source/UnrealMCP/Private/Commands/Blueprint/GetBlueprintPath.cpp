﻿#include "Commands/Blueprint/GetBlueprintPath.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FGetBlueprintPathCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString Path = FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName);

		if (Path.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("path"), Path);
		});
	}

}
