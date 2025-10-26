#include "Commands/Registry/GetSupportedParentClasses.h"
#include "Core/MCPRegistry.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	TSharedPtr<FJsonObject> FGetSupportedParentClassesCommand::Execute(const TSharedPtr<FJsonObject>& Params) {
		TArray<FString> ClassNames;

		if (const FVoidResult Result = FMCPRegistry::GetSupportedParentClasses(ClassNames); !Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			TArray<TSharedPtr<FJsonValue>> JsonArray;
			for (const FString& ClassName : ClassNames) {
				JsonArray.Add(MakeShared<FJsonValueString>(ClassName));
			}
			Data->SetArrayField(TEXT("classes"), JsonArray);
			Data->SetNumberField(TEXT("count"), ClassNames.Num());
		});
	}

} // namespace UnrealMCP
