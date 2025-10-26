#include "Commands/Blueprint/DeleteBlueprint.h"
#include "Services/BlueprintService.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	auto FDeleteBlueprintCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse and validate parameters using the service
		const auto DeleteParams = FDeleteBlueprintParams::FromJson(Params);
		if (!DeleteParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(DeleteParams.GetError());
		}

		// Delegate to service layer
		const auto Result = FBlueprintService::DeleteBlueprint(DeleteParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		// Format response
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			const auto& DeleteResult = Result.GetValue();
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Blueprint '%s' deleted successfully"), *DeleteParams.GetValue().BlueprintName)
			);
			Data->SetStringField(TEXT("deleted_path"), DeleteResult.DeletedPath);
		});
	}

} // namespace UnrealMCP
