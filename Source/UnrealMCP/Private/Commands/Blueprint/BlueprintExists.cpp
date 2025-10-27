#include "Commands/Blueprint/BlueprintExists.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FBlueprintExistsCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const bool bExists = FBlueprintIntrospectionService::BlueprintExists(BlueprintName);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetBoolField(TEXT("exists"), bExists);
			Data->SetStringField(TEXT("blueprint_name"), BlueprintName);
		});
	}

}
