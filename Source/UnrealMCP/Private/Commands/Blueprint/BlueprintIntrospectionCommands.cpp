#include "Commands/Blueprint/BlueprintIntrospectionCommands.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	auto FBlueprintExistsCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const bool bExists = FBlueprintIntrospectionService::BlueprintExists(BlueprintName);

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetBoolField(TEXT("exists"), bExists);
			Data->SetStringField(TEXT("blueprint_name"), BlueprintName);
		});
	}

	auto FGetBlueprintInfoCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		TMap<FString, FString> Info;

		if (const FVoidResult Result = FBlueprintIntrospectionService::GetBlueprintInfo(BlueprintName, Info); !Result.IsSuccess()) {
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

	auto FGetBlueprintComponentsCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		TArray<TMap<FString, FString>> Components;

		if (const FVoidResult Result = FBlueprintIntrospectionService::GetBlueprintComponents(BlueprintName, Components); !Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			TArray<TSharedPtr<FJsonValue>> JsonArray;
			for (const TMap<FString, FString>& ComponentInfo : Components) {
				TSharedPtr<FJsonObject> ComponentObject = MakeShared<FJsonObject>();
				for (const auto& Pair : ComponentInfo) {
					ComponentObject->SetStringField(Pair.Key, Pair.Value);
				}
				JsonArray.Add(MakeShared<FJsonValueObject>(ComponentObject));
			}
			Data->SetArrayField(TEXT("components"), JsonArray);
			Data->SetNumberField(TEXT("count"), Components.Num());
		});
	}

	auto FGetBlueprintVariablesCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		TArray<TMap<FString, FString>> Variables;

		if (const FVoidResult Result = FBlueprintIntrospectionService::GetBlueprintVariables(BlueprintName, Variables); !Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		TArray<TSharedPtr<FJsonValue>> JsonArray;
		for (const TMap<FString, FString>& VarInfo : Variables) {
			TSharedPtr<FJsonObject> VarObject = MakeShared<FJsonObject>();
			for (const auto& Pair : VarInfo) {
				VarObject->SetStringField(Pair.Key, Pair.Value);
			}
			JsonArray.Add(MakeShared<FJsonValueObject>(VarObject));
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetArrayField(TEXT("variables"), JsonArray);
			Data->SetNumberField(TEXT("count"), Variables.Num());
		});
	}

	auto FGetBlueprintPathCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString Path = FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName);

		if (Path.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("path"), Path);
		});
	}

} // namespace UnrealMCP
