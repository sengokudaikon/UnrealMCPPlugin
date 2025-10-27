#include "Commands/Blueprint/CreateBlueprint.h"

#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/BlueprintCreationService.h"

namespace UnrealMCP {
	auto FCreateBlueprint::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		TResult<FBlueprintCreationParams> ParamsResult =
			FBlueprintCreationParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const TResult<UBlueprint*> Result =
			FBlueprintCreationService::CreateBlueprint(ParamsResult.GetValue());

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		const FBlueprintCreationParams& ParsedParams = ParamsResult.GetValue();
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
}
