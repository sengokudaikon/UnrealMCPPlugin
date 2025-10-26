#include "Commands/Blueprint/RenameComponent.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"

namespace UnrealMCP {

	auto FRenameComponentCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		auto ParseResult = FRenameComponentParams::FromJson(Params);
		if (ParseResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParseResult.GetError());
		}

		const FRenameComponentParams& RenameParams = ParseResult.GetValue();
		auto Result = FBlueprintIntrospectionService::renameComponent(RenameParams);
		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		const FRenameComponentResult& RenameResult = Result.GetValue();
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetObjectField(TEXT("result"), RenameResult.ToJson());
		});
	}

}
