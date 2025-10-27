#include "Commands/Blueprint/DeleteBlueprint.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintService.h"

namespace UnrealMCP {

	auto FDeleteBlueprintCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		const auto DeleteParams = FDeleteBlueprintParams::FromJson(Params);
		if (!DeleteParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(DeleteParams.GetError());
		}

		const auto Result = FBlueprintService::DeleteBlueprint(DeleteParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			const auto& [DeletedPath] = Result.GetValue();
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Blueprint '%s' deleted successfully"), *DeleteParams.GetValue().BlueprintName)
			);
			Data->SetStringField(TEXT("deleted_path"), DeletedPath);
		});
	}

}
