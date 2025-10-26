#include "Commands/Registry/GetAvailableAPIMethods.h"
#include "Core/MCPRegistry.h"
#include "Commands/CommonUtils.h"

namespace UnrealMCP {

	TSharedPtr<FJsonObject> FGetAvailableAPIMethodsCommand::Execute(const TSharedPtr<FJsonObject>& Params) {
		TMap<FString, TArray<FString>> Methods;
		const FVoidResult Result = FMCPRegistry::GetAvailableAPIMethods(Methods);

		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		// Convert to JSON structure
		const TSharedPtr<FJsonObject> MethodsObject = MakeShared<FJsonObject>();
		int32 TotalCount = 0;

		for (const auto& Category : Methods) {
			TArray<TSharedPtr<FJsonValue>> MethodArray;
			for (const FString& Method : Category.Value) {
				MethodArray.Add(MakeShared<FJsonValueString>(Method));
			}
			MethodsObject->SetArrayField(Category.Key, MethodArray);
			TotalCount += Category.Value.Num();
		}

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetObjectField(TEXT("methods"), MethodsObject);
		Response->SetNumberField(TEXT("total_methods"), TotalCount);
		Response->SetNumberField(TEXT("categories"), Methods.Num());

		return Response;
	}

} // namespace UnrealMCP
