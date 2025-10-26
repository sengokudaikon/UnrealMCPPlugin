#include "Commands/Registry/GetSupportedParentClasses.h"
#include "Core/MCPRegistry.h"
#include "Commands/CommonUtils.h"

namespace UnrealMCP {

	TSharedPtr<FJsonObject> FGetSupportedParentClassesCommand::Execute(const TSharedPtr<FJsonObject>& Params) {
		TArray<FString> ClassNames;
		const FVoidResult Result = FMCPRegistry::GetSupportedParentClasses(ClassNames);

		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		// Convert to JSON array
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		for (const FString& ClassName : ClassNames) {
			JsonArray.Add(MakeShared<FJsonValueString>(ClassName));
		}

		TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
		Response->SetBoolField(TEXT("success"), true);
		Response->SetArrayField(TEXT("classes"), JsonArray);
		Response->SetNumberField(TEXT("count"), ClassNames.Num());

		return Response;
	}

} // namespace UnrealMCP
