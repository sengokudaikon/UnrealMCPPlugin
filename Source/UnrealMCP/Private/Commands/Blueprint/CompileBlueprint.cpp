#include "Commands/Blueprint/CompileBlueprint.h"

#include "Commands/CommonUtils.h"
#include "Core/Result.h"
#include "Services/BlueprintCreationService.h"

auto FCompileBlueprint::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
	}

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FBlueprintCreationService::CompileBlueprint(BlueprintName);

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("name"), BlueprintName);
	Response->SetBoolField(TEXT("compiled"), true);
	return Response;
}
