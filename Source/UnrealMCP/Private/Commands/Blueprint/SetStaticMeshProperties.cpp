#include "Commands/Blueprint/SetStaticMeshProperties.h"

#include "Core/CommonUtils.h"
#include "Core/MCPTypes.h"
#include "Core/Result.h"
#include "Services/BlueprintService.h"

auto FSetStaticMeshProperties::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

	UnrealMCP::TResult<UnrealMCP::FStaticMeshParams> ParamsResult =
		UnrealMCP::FStaticMeshParams::FromJson(Params);

	if (ParamsResult.IsFailure()) {
		return FCommonUtils::CreateErrorResponse(ParamsResult.GetError());
	}

	const UnrealMCP::FStaticMeshParams& MeshParams = ParamsResult.GetValue();

	const UnrealMCP::FVoidResult Result =
		UnrealMCP::FBlueprintService::SetStaticMeshProperties(
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
