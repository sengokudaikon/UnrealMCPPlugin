#include "Commands/Blueprint/SetStaticMeshProperties.h"

#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

namespace UnrealMCP {

	auto FSetStaticMeshProperties::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		TResult<FStaticMeshParams> ParamsResult =
			FStaticMeshParams::FromJson(Params);

		if (ParamsResult.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
		}

		const FStaticMeshParams& MeshParams = ParamsResult.GetValue();

		const FVoidResult Result =
			FBlueprintService::SetStaticMeshProperties(
				MeshParams.BlueprintName,
				MeshParams.ComponentName,
				MeshParams.StaticMesh,
				MeshParams.Material
			);

		if (Result.IsFailure()) {
			return FCommonUtils::CreateErrorResponse(Result.GetError());
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetStringField(TEXT("component"), MeshParams.ComponentName);
		});
	}

}
