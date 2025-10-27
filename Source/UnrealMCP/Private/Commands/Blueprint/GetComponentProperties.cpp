#include "Commands/Blueprint/GetComponentProperties.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FGetComponentPropertiesCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		const auto ComponentParams = FComponentPropertiesParams::FromJson(Params);
		if (!ComponentParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(ComponentParams.GetError());
		}

		const auto Result = FBlueprintIntrospectionService::GetComponentProperties(ComponentParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			const auto& ComponentResult = Result.GetValue();
			Data->SetObjectField(TEXT("properties"), ComponentResult.Properties);
		});
	}

}
