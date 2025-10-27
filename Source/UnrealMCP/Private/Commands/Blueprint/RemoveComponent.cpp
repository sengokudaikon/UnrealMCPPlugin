#include "Commands/Blueprint/RemoveComponent.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FRemoveComponentCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		const auto RemoveParams = FRemoveComponentParams::FromJson(Params);
		if (!RemoveParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(RemoveParams.GetError());
		}

		const auto Result = FBlueprintIntrospectionService::RemoveComponent(RemoveParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			const auto& RemoveResult = Result.GetValue();
			Data->SetStringField(TEXT("blueprint_name"), RemoveResult.BlueprintName);
			Data->SetStringField(TEXT("component_name"), RemoveResult.ComponentName);
			Data->SetStringField(TEXT("message"), RemoveResult.Message);
		});
	}

}
