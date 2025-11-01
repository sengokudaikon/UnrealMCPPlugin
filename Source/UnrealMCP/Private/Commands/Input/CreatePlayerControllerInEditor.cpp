#include "Commands/Input/CreatePlayerControllerInEditor.h"
#include "Core/CommonUtils.h"
#include "Services/InputService.h"

namespace UnrealMCP {

	auto FCreatePlayerControllerInEditor::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		const FVoidResult Result = FInputService::CreatePlayerControllerInEditor();

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("message"), TEXT("PlayerController created successfully in editor world"));
		});
	}
}