#include "Commands/Blueprint/CompileBlueprint.h"

#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintCreationService.h"

namespace UnrealMCP {
	auto FCompileBlueprint::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		FString BlueprintName;
		if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing 'blueprint_name' parameter")));
		}

		const FVoidResult Result =
			FBlueprintCreationService::CompileBlueprint(BlueprintName);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("name"), BlueprintName);
			Data->SetBoolField(TEXT("compiled"), true);
		});
	}
}
