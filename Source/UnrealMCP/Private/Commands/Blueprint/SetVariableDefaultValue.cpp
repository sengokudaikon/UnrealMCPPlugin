#include "Commands/Blueprint/SetVariableDefaultValue.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Services/BlueprintMemberService.h"

namespace UnrealMCP {

	auto FSetVariableDefaultValueCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("variable_name")) ||
			!Params->HasField(TEXT("value"))) {
			return FCommonUtils::CreateErrorResponse(
				FError(EErrorCode::InvalidInput, TEXT("Missing required parameters: blueprint_name, variable_name, and value"))
			);
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString VariableName = Params->GetStringField(TEXT("variable_name"));
		const TSharedPtr<FJsonValue> Value = Params->TryGetField(TEXT("value"));


		const FVoidResult Result = FBlueprintMemberService::SetVariableDefaultValue(
			BlueprintName,
			VariableName,
			Value
		);


		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(
				TEXT("message"),
				FString::Printf(
					TEXT("Default value set for variable '%s' in blueprint '%s'"),
					*VariableName,
					*BlueprintName
				)
			);
			Data->SetStringField(TEXT("variable_name"), VariableName);
		});
	}

}
