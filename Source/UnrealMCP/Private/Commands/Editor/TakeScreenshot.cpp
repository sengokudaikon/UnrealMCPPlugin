#include "Commands/Editor/TakeScreenshot.h"
#include "Core/CommonUtils.h"
#include "Services/ViewportService.h"
#include "Core/MCPTypes.h"

namespace UnrealMCP {

auto FTakeScreenshot::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString FilePath;
	if (!Params->TryGetStringField(TEXT("filepath"), FilePath)) {
		FilePath = FPaths::ProjectSavedDir() / TEXT("Screenshots") / FString::Printf(TEXT("Screenshot_%s.png"), *FDateTime::Now().ToString());
	}

	const TResult<FString> Result = FViewportService::TakeScreenshot(FilePath);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("filepath"), Result.GetValue());
	});
}}
