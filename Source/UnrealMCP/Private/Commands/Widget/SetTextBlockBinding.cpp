#include "Commands/UMG/SetTextBlockBinding.h"
#include "Core/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"

auto FSetTextBlockBinding::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	UnrealMCP::TResult<UnrealMCP::FTextBlockBindingParams> ParamsResult =
		UnrealMCP::FTextBlockBindingParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FWidgetService::SetTextBlockBinding(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const UnrealMCP::FTextBlockBindingParams& ParsedParams = ParamsResult.GetValue();
	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("text_block_name"), ParsedParams.TextBlockName);
		Data->SetStringField(TEXT("binding_property"), ParsedParams.BindingProperty);
	});
}