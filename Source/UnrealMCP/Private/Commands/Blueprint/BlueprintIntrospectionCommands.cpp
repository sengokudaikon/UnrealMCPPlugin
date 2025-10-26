#include "Commands/Blueprint/BlueprintIntrospectionCommands.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Commands/CommonUtils.h"

namespace UnrealMCP {

	auto FBlueprintExistsCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const bool bExists = FBlueprintIntrospectionService::BlueprintExists(BlueprintName);

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetBoolField(TEXT("exists"), bExists);
		Response->SetStringField(TEXT("blueprint_name"), BlueprintName);

		return Response;
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

		const TSharedPtr<FJsonObject> InfoObject = MakeShared<FJsonObject>();
		for (const auto& Pair : Info) {
			InfoObject->SetStringField(Pair.Key, Pair.Value);
		}

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetObjectField(TEXT("info"), InfoObject);

		return Response;
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

		TArray<TSharedPtr<FJsonValue>> JsonArray;
		for (const TMap<FString, FString>& ComponentInfo : Components) {
			TSharedPtr<FJsonObject> ComponentObject = MakeShared<FJsonObject>();
			for (const auto& Pair : ComponentInfo) {
				ComponentObject->SetStringField(Pair.Key, Pair.Value);
			}
			JsonArray.Add(MakeShared<FJsonValueObject>(ComponentObject));
		}

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetArrayField(TEXT("components"), JsonArray);
		Response->SetNumberField(TEXT("count"), Components.Num());

		return Response;
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

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetArrayField(TEXT("variables"), JsonArray);
		Response->SetNumberField(TEXT("count"), Variables.Num());

		return Response;
	}

	auto FGetBlueprintPathCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		if (!Params->HasField(TEXT("blueprint_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing blueprint_name parameter"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString Path = FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName);

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		if (Path.IsEmpty()) {
			Response->SetBoolField(TEXT("success"), false);
			Response->SetStringField(TEXT("message"), FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		} else {
			Response->SetBoolField(TEXT("success"), true);
			Response->SetStringField(TEXT("path"), Path);
		}

		return Response;
	}

} // namespace UnrealMCP
