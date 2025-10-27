#include "Types/BlueprintIntrospectionTypes.h"

namespace UnrealMCP {
	auto FBlueprintFunctionParam::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Param = MakeShared<FJsonObject>();
		Param->SetStringField(TEXT("name"), Name);
		Param->SetStringField(TEXT("type"), Type);
		if (SubType.IsSet()) {
			Param->SetStringField(TEXT("subtype"), SubType.GetValue());
		}
		Param->SetBoolField(TEXT("is_array"), bIsArray);
		Param->SetBoolField(TEXT("is_reference"), bIsReference);
		return Param;
	}

	auto FBlueprintFunctionInfo::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Function = MakeShared<FJsonObject>();
		Function->SetStringField(TEXT("name"), Name);
		Function->SetStringField(TEXT("category"), Category);
		Function->SetStringField(TEXT("tooltip"), Tooltip);
		Function->SetStringField(TEXT("keywords"), Keywords);
		Function->SetBoolField(TEXT("is_pure"), bIsPure);
		Function->SetNumberField(TEXT("node_count"), NodeCount);

		TArray<TSharedPtr<FJsonValue>> ParamsArray;
		for (const FBlueprintFunctionParam& Param : Parameters) {
			ParamsArray.Add(MakeShared<FJsonValueObject>(Param.ToJson()));
		}
		Function->SetArrayField(TEXT("parameters"), ParamsArray);

		TArray<TSharedPtr<FJsonValue>> ReturnsArray;
		for (const FBlueprintFunctionParam& Param : Returns) {
			ReturnsArray.Add(MakeShared<FJsonValueObject>(Param.ToJson()));
		}
		Function->SetArrayField(TEXT("returns"), ReturnsArray);

		return Function;
	}

	auto FGetBlueprintFunctionsResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		Result->SetNumberField(TEXT("count"), Count);

		TArray<TSharedPtr<FJsonValue>> FunctionsArray;
		for (const FBlueprintFunctionInfo& Function : Functions) {
			FunctionsArray.Add(MakeShared<FJsonValueObject>(Function.ToJson()));
		}
		Result->SetArrayField(TEXT("functions"), FunctionsArray);

		return Result;
	}

	auto FBlueprintVariableInfo::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Variable = MakeShared<FJsonObject>();
		Variable->SetStringField(TEXT("name"), Name);
		Variable->SetStringField(TEXT("type"), Type);
		Variable->SetStringField(TEXT("category"), Category);
		Variable->SetStringField(TEXT("tooltip"), Tooltip);
		Variable->SetBoolField(TEXT("is_array"), bIsArray);
		Variable->SetBoolField(TEXT("is_reference"), bIsReference);
		Variable->SetBoolField(TEXT("instance_editable"), bInstanceEditable);
		Variable->SetBoolField(TEXT("blueprint_readonly"), bBlueprintReadOnly);
		Variable->SetBoolField(TEXT("expose_on_spawn"), bExposeOnSpawn);
		Variable->SetStringField(TEXT("default_value"), DefaultValue);
		return Variable;
	}

	auto FGetBlueprintVariablesResult::ToJson() const -> TSharedPtr<FJsonObject> {
		auto Result = MakeShared<FJsonObject>();
		Result->SetNumberField(TEXT("count"), Count);

		TArray<TSharedPtr<FJsonValue>> VariablesArray;
		for (const FBlueprintVariableInfo& Variable : Variables) {
			VariablesArray.Add(MakeShared<FJsonValueObject>(Variable.ToJson()));
		}
		Result->SetArrayField(TEXT("variables"), VariablesArray);

		return Result;
	}
}
