#include "Commands/Blueprint/SetFunctionMetadata.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FSetFunctionMetadataCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name and function_name")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));

		TOptional<FString> Category;
		if (Params->HasField(TEXT("category"))) {
			Category = Params->GetStringField(TEXT("category"));
		}

		TOptional<FString> Tooltip;
		if (Params->HasField(TEXT("tooltip"))) {
			Tooltip = Params->GetStringField(TEXT("tooltip"));
		}

		TOptional<bool> bPure;
		if (Params->HasField(TEXT("pure"))) {
			bPure = Params->GetBoolField(TEXT("pure"));
		}


		const FVoidResult Result = FBlueprintMemberService::SetFunctionMetadata(
			BlueprintName,
			FunctionName,
			Category,
			Tooltip,
			bPure
		);


		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(TEXT("Metadata set for function '%s' in blueprint '%s'"), *FunctionName, *BlueprintName)
			);
		});
	}

}
