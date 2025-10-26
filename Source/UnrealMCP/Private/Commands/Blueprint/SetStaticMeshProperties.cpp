#include "Commands/Blueprint/SetStaticMeshProperties.h"

#include "Commands/CommonUtils.h"
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

	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("component"), MeshParams.ComponentName);
	Response->SetBoolField(TEXT("success"), true);
	return Response;
}
