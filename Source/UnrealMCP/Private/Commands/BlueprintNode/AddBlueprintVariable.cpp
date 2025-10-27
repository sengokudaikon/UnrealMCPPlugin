#include "Commands/BlueprintNode/AddBlueprintVariable.h"
#include "Core/CommonUtils.h"
#include "Services/BlueprintGraphService.h"

namespace UnrealMCP {
	auto FAddBlueprintVariable::Handle(
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {

		FString BlueprintName;
		if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
		}

		FString VariableName;
		if (!Params->TryGetStringField(TEXT("variable_name"), VariableName)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'variable_name' parameter"));
		}

		FString VariableType;
		if (!Params->TryGetStringField(TEXT("variable_type"), VariableType)) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'variable_type' parameter"));
		}

		bool IsExposed = false;
		if (Params->HasField(TEXT("is_exposed"))) {
			IsExposed = Params->GetBoolField(TEXT("is_exposed"));
		}

		const FVoidResult Result = FBlueprintGraphService::AddVariable(
			BlueprintName,
			VariableName,
			VariableType,
			IsExposed
		);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}


		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("variable_name"), VariableName);
			Data->SetStringField(TEXT("variable_type"), VariableType);
		});
	}
}
