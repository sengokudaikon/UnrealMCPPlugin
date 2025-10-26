#include "Commands/Widget/SetTextBlockBinding.h"
#include "Core/CommonUtils.h"
#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"

namespace UnrealMCP {

auto FSetTextBlockBinding::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	TResult<FTextBlockBindingParams> ParamsResult =
		FTextBlockBindingParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const FVoidResult Result =
		FWidgetService::SetTextBlockBinding(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	const FTextBlockBindingParams& ParsedParams = ParamsResult.GetValue();
	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("text_block_name"), ParsedParams.TextBlockName);
		Data->SetStringField(TEXT("binding_property"), ParsedParams.BindingProperty);
	});
}}