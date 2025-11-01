#include "Commands/Blueprint/GetBlueprintInfo.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FGetBlueprintInfoCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing blueprint_name parameter")));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		TMap<FString, FString> Info;

		if (const FVoidResult Result = FBlueprintIntrospectionService::GetBlueprintInfo(BlueprintName, Info); !Result.
			IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			const TSharedPtr<FJsonObject> InfoObject = MakeShared<FJsonObject>();
			for (const auto& Pair : Info) {
				InfoObject->SetStringField(Pair.Key, Pair.Value);
			}
			Data->SetObjectField(TEXT("info"), InfoObject);
		});
	}

}
