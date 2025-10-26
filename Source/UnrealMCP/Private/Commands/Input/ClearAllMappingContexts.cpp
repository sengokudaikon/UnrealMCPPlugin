#include "Commands/Input/ClearAllMappingContexts.h"
#include "Core/CommonUtils.h"
#include "Services/InputService.h"
#include "Core/MCPTypes.h"

auto FClearAllMappingContexts::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	// No parameter parsing needed for this command

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FInputService::ClearAllMappingContexts();

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetBoolField(TEXT("success"), true);
	});
}
