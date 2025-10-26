#include "Commands/Blueprint/GetComponentHierarchy.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	auto FGetComponentHierarchyCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse and validate parameters using the service
		const auto HierarchyParams = FComponentHierarchyParams::FromJson(Params);
		if (!HierarchyParams.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(HierarchyParams.GetError());
		}

		// Delegate to service layer
		const auto Result = FBlueprintIntrospectionService::GetComponentHierarchy(HierarchyParams.GetValue());
		if (!Result.IsSuccess()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		// Format response using the service result
		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			const auto& HierarchyResult = Result.GetValue();
			Data->SetArrayField(TEXT("hierarchy"), HierarchyResult.Hierarchy);
			Data->SetNumberField(TEXT("root_count"), HierarchyResult.RootCount);
			Data->SetNumberField(TEXT("total_components"), HierarchyResult.TotalComponents);
		});
	}

} // namespace UnrealMCP
