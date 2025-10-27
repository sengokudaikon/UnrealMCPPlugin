#include "Commands/Blueprint/GetComponentHierarchy.h"
#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Services/BlueprintIntrospectionService.h"

namespace UnrealMCP {

	auto FGetComponentHierarchyCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		const auto HierarchyParams = FComponentHierarchyParams::FromJson(Params);
		if (!HierarchyParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(HierarchyParams.GetError());
		}

		const auto Result = FBlueprintIntrospectionService::GetComponentHierarchy(HierarchyParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			const auto& HierarchyResult = Result.GetValue();
			Data->SetArrayField(TEXT("hierarchy"), HierarchyResult.Hierarchy);
			Data->SetNumberField(TEXT("root_count"), HierarchyResult.RootCount);
			Data->SetNumberField(TEXT("total_components"), HierarchyResult.TotalComponents);
		});
	}

}
