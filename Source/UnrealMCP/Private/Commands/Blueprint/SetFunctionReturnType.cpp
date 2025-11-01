#include "Commands/Blueprint/SetFunctionReturnType.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FSetFunctionReturnTypeCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse required parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name")) ||
			!Params->HasField(TEXT("return_type"))) {
			return FCommonUtils::CreateErrorResponse(
				FError(EErrorCode::InvalidInput, TEXT("Missing required parameters: blueprint_name, function_name, and return_type"))
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));
		const FString ReturnType = Params->GetStringField(TEXT("return_type"));


		const FVoidResult Result = FBlueprintMemberService::SetFunctionReturnType(
			BlueprintName,
			FunctionName,
			ReturnType
		);


		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(
					TEXT("Return type '%s' set for function '%s' in blueprint '%s'"),
					*ReturnType,
					*FunctionName,
					*BlueprintName
				)
			);
		});
	}

}
