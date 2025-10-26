#include "Commands/UMG/CreateUMGWidgetBlueprint.h"
#include "Core/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"
#include "WidgetBlueprint.h"

auto FCreateUMGWidgetBlueprint::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FWidgetCreationParams> ParamsResult =
		UnrealMCP::FWidgetCreationParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::TResult<UWidgetBlueprint*> Result =
		UnrealMCP::FWidgetService::CreateWidget(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const UnrealMCP::FWidgetCreationParams& ParsedParams = ParamsResult.GetValue();
	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("name"), ParsedParams.Name);
		Data->SetStringField(TEXT("path"), ParsedParams.PackagePath / ParsedParams.Name);
	});
}