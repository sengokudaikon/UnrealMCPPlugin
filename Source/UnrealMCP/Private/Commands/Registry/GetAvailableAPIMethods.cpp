#include "Commands/Registry/GetAvailableAPIMethods.h"
#include "Core/CommonUtils.h"
#include "Core/MCPRegistry.h"

namespace UnrealMCP {

	auto FGetAvailableAPIMethodsCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		TMap<FString, TArray<FString>> Methods;

		if (const FVoidResult Result = FMCPRegistry::GetAvailableAPIMethods(Methods); !Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
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

			Data->SetObjectField(TEXT("methods"), MethodsObject);
			Data->SetNumberField(TEXT("total_methods"), TotalCount);
			Data->SetNumberField(TEXT("categories"), Methods.Num());
		});
	}

}
