#include "Commands/Editor/FocusViewport.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Core/MCPTypes.h"
#include "Services/ViewportService.h"

namespace UnrealMCP {

	auto FFocusViewport::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		TOptional<FString> TargetActor;
		if (Params->HasField(TEXT("target"))) {
			if (FString Target; Params->TryGetStringField(TEXT("target"), Target)) {
				TargetActor = Target;
			}
		}

		TOptional<FVector> Location;
		if (Params->HasField(TEXT("location"))) {
			Location = FCommonUtils::GetVectorFromJson(Params, TEXT("location"));
		}

		if (!TargetActor.IsSet() && !Location.IsSet()) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Either 'target' or 'location' must be provided")));
		}

		const FVoidResult Result = FViewportService::FocusViewport(
			TargetActor,
			Location
		);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetBoolField(TEXT("success"), true);

			if (TargetActor.IsSet()) {
				Data->SetStringField(TEXT("focused_on"), TargetActor.GetValue());
			}
		});
	}
}
