#include "Commands/Widget/CreateUMGWidgetBlueprint.h"
#include "WidgetBlueprint.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/WidgetService.h"

namespace UnrealMCP {

	auto FCreateUMGWidgetBlueprint::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		TResult<FWidgetCreationParams> ParamsResult =
			FWidgetCreationParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const TResult<UWidgetBlueprint*> Result =
			FWidgetService::CreateWidget(ParamsResult.GetValue());

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		const FWidgetCreationParams& ParsedParams = ParamsResult.GetValue();
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("name"), ParsedParams.Name);
			Data->SetStringField(TEXT("path"), ParsedParams.PackagePath / ParsedParams.Name);
		});
	}
}
