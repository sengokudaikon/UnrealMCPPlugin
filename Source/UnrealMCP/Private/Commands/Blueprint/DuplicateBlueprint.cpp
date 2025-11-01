#include "Commands/Blueprint/DuplicateBlueprint.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/CommonUtils.h"
#include "Core/ErrorTypes.h"
#include "Engine/Blueprint.h"
#include "Misc/PackageName.h"
#include "Services/BlueprintIntrospectionService.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

namespace UnrealMCP {

	auto FDuplicateBlueprintCommand::Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {

		if (!Params->HasField(TEXT("source_name")) || !Params->HasField(TEXT("new_name"))) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("Missing required parameters: source_name and new_name")));
		}

		const FString SourceName = Params->GetStringField(TEXT("source_name"));
		const FString NewName = Params->GetStringField(TEXT("new_name"));
		const FString PackagePath =
			Params->HasField(TEXT("path")) ? Params->GetStringField(TEXT("path")) : TEXT("/Game/Blueprints/");

		if (NewName.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(FError(EErrorCode::InvalidInput, TEXT("New blueprint name cannot be empty")));
		}

		const FString SourcePath = FBlueprintIntrospectionService::GetBlueprintPath(SourceName);
		if (SourcePath.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(
				FError(EErrorCode::BlueprintNotFound, FString::Printf(TEXT("Source blueprint '%s' not found"), *SourceName))
			);
		}

		UBlueprint* SourceBlueprint = LoadObject<UBlueprint>(nullptr, *SourcePath);
		if (!SourceBlueprint) {
			return FCommonUtils::CreateErrorResponse(
				FError(EErrorCode::BlueprintNotFound, FString::Printf(TEXT("Failed to load source blueprint '%s'"), *SourceName))
			);
		}

		const FString NewPackagePath = FString::Printf(TEXT("%s%s"), *PackagePath, *NewName);

		if (const FString NewAssetPath = FString::Printf(TEXT("%s.%s"), *NewPackagePath, *NewName);
			FBlueprintIntrospectionService::BlueprintExists(NewAssetPath)) {
			return FCommonUtils::CreateErrorResponse(
				FError(EErrorCode::InvalidInput, FString::Printf(TEXT("Blueprint '%s' already exists at path '%s'"), *NewName, *NewPackagePath))
			);
		}

		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		IAssetTools& AssetTools = AssetToolsModule.Get();

		TArray<UObject*> ObjectsToDuplicate;
		ObjectsToDuplicate.Add(SourceBlueprint);

		TArray<UObject*> NewObjects;
		ObjectTools::DuplicateObjects(ObjectsToDuplicate, TEXT(""), PackagePath, false, &NewObjects);

		if (NewObjects.Num() > 0) {
			if (UBlueprint* NewBlueprint = Cast<UBlueprint>(NewObjects[0])) {
				UPackage* Package = NewBlueprint->GetOutermost();

				FAssetRenameData RenameData(NewBlueprint, PackagePath, NewName);
				TArray<FAssetRenameData> AssetsToRename;
				AssetsToRename.Add(RenameData);

				AssetTools.RenameAssets(AssetsToRename);

				if (FString PackageFileName; FPackageName::TryConvertLongPackageNameToFilename(
					NewPackagePath,
					PackageFileName,
					FPackageName::GetAssetPackageExtension()
				)) {
					FSavePackageArgs SaveArgs;
					SaveArgs.SaveFlags = RF_Public;
					UPackage::SavePackage(Package, NewBlueprint, *PackageFileName, SaveArgs);
				}

				FAssetRegistryModule& AssetRegistryModule =
					FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				AssetRegistryModule.Get().AssetCreated(NewBlueprint);

				return FCommonUtils::CreateSuccessResponse([&](TSharedPtr<FJsonObject>& Data) {
					Data->SetStringField(TEXT("message"), TEXT("Blueprint duplicated successfully"));
					Data->SetStringField(TEXT("source_name"), SourceName);
					Data->SetStringField(TEXT("source_path"), SourcePath);
					Data->SetStringField(TEXT("new_name"), NewName);
					Data->SetStringField(TEXT("new_path"), NewBlueprint->GetPathName());
				});
			}
		}

		return FCommonUtils::CreateErrorResponse(FError(EErrorCode::OperationFailed, TEXT("Failed to duplicate blueprint")));
	}

}
