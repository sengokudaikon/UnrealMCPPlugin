#include "Commands/Blueprint/CreateBlueprint.h"

#include "Core/CommonUtils.h"
#include "Services/BlueprintCreationService.h"
#include "Core/MCPTypes.h"

auto FCreateBlueprint::Handle(
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FBlueprintCreationParams> ParamsResult =
		UnrealMCP::FBlueprintCreationParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::TResult<UBlueprint*> Result =
		UnrealMCP::FBlueprintCreationService::CreateBlueprint(ParamsResult.GetValue());

	if (Result.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(Result.GetError());
	}


	const UnrealMCP::FBlueprintCreationParams& ParsedParams = ParamsResult.GetValue();
	const UBlueprint* Blueprint = Result.GetValue();

	return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
		Data->SetStringField(TEXT("name"), ParsedParams.Name);
		Data->SetStringField(TEXT("path"), ParsedParams.PackagePath + ParsedParams.Name);
		Data->SetStringField(
			TEXT("parent_class"),
			Blueprint->ParentClass ? Blueprint->ParentClass->GetName() : TEXT("AActor")
		);
	});
}
