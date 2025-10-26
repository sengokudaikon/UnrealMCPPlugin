#include "Commands/Blueprint/AddFunctionParameter.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FAddFunctionParameterCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse required parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("function_name")) ||
			!Params->HasField(TEXT("param_name")) || !Params->HasField(TEXT("param_type"))) {
			return FCommonUtils::CreateErrorResponse(
				TEXT("Missing required parameters: blueprint_name, function_name, param_name, and param_type")
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString FunctionName = Params->GetStringField(TEXT("function_name"));
		const FString ParamName = Params->GetStringField(TEXT("param_name"));
		const FString ParamType = Params->GetStringField(TEXT("param_type"));
		const bool bIsReference = Params->HasField(TEXT("is_reference"))
			? Params->GetBoolField(TEXT("is_reference"))
			: false;


		const FVoidResult Result = FBlueprintMemberService::AddFunctionParameter(
			BlueprintName,
			FunctionName,
			ParamName,
			ParamType,
			bIsReference
		);


		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(
					TEXT("Parameter '%s' of type '%s' added to function '%s' in blueprint '%s'"),
					*ParamName,
					*ParamType,
					*FunctionName,
					*BlueprintName
				)
			);
		});
	}
}
