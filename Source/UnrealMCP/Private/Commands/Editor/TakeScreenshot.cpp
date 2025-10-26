#include "Commands/Editor/TakeScreenshot.h"
#include "Commands/CommonUtils.h"
#include "Services/ViewportService.h"
#include "Core/MCPTypes.h"

auto FTakeScreenshot::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	FString FilePath;
	if (!Params->TryGetStringField(TEXT("filepath"), FilePath)) {
		// Use default path if not provided
		FilePath = FPaths::ProjectSavedDir() / TEXT("Screenshots") / FString::Printf(TEXT("Screenshot_%s.png"), *FDateTime::Now().ToString());
	}

	const UnrealMCP::TResult<FString> Result = UnrealMCP::FViewportService::TakeScreenshot(FilePath);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("filepath"), Result.GetValue());
	return Response;
}
