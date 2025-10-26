#include "Commands/Blueprint/DuplicateBlueprint.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Core/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "Factories/BlueprintFactory.h"
#include "IAssetTools.h"
#include "ObjectTools.h"
#include "Services/BlueprintIntrospectionService.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"

namespace UnrealMCP {

	auto FDuplicateBlueprintCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse parameters
		if (!Params->HasField(TEXT("source_name")) || !Params->HasField(TEXT("new_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing required parameters: source_name and new_name"));
		}

		const FString SourceName = Params->GetStringField(TEXT("source_name"));
		const FString NewName = Params->GetStringField(TEXT("new_name"));
		const FString PackagePath =
			Params->HasField(TEXT("path")) ? Params->GetStringField(TEXT("path")) : TEXT("/Game/Blueprints/");

		// Validate new name
		if (NewName.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(TEXT("New blueprint name cannot be empty"));
		}

		// Find the source blueprint
		const FString SourcePath = FBlueprintIntrospectionService::GetBlueprintPath(SourceName);
		if (SourcePath.IsEmpty()) {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Source blueprint '%s' not found"), *SourceName)
			);
		}

		UBlueprint* SourceBlueprint = LoadObject<UBlueprint>(nullptr, *SourcePath);
		if (!SourceBlueprint) {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Failed to load source blueprint '%s'"), *SourceName)
			);
		}

		// Create the new package path
		const FString NewPackagePath = FString::Printf(TEXT("%s%s"), *PackagePath, *NewName);
		const FString NewAssetPath = FString::Printf(TEXT("%s.%s"), *NewPackagePath, *NewName);

		// Check if blueprint already exists
		if (FBlueprintIntrospectionService::BlueprintExists(NewAssetPath)) {
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Blueprint '%s' already exists at path '%s'"), *NewName, *NewPackagePath)
			);
		}

		// Get asset tools
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		IAssetTools& AssetTools = AssetToolsModule.Get();

		// Duplicate the asset
		TArray<UObject*> ObjectsToDuplicate;
		ObjectsToDuplicate.Add(SourceBlueprint);

		TArray<UObject*> NewObjects;
		ObjectTools::DuplicateObjects(ObjectsToDuplicate, TEXT(""), PackagePath, false, &NewObjects);

		if (NewObjects.Num() > 0) {
			if (UBlueprint* NewBlueprint = Cast<UBlueprint>(NewObjects[0])) {
				// Rename the duplicated asset
				UPackage* Package = NewBlueprint->GetOutermost();

				// Use the asset tools to properly rename
				FAssetRenameData RenameData(NewBlueprint, PackagePath, NewName);
				TArray<FAssetRenameData> AssetsToRename;
				AssetsToRename.Add(RenameData);

				AssetTools.RenameAssets(AssetsToRename);

				// Save the package
				if (FString PackageFileName; FPackageName::TryConvertLongPackageNameToFilename(
						NewPackagePath, PackageFileName, FPackageName::GetAssetPackageExtension()
					)) {
					FSavePackageArgs SaveArgs;
					SaveArgs.SaveFlags = EObjectFlags::RF_Public;
					UPackage::SavePackage(Package, NewBlueprint, *PackageFileName, SaveArgs);
				}

				// Notify asset registry
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

		return FCommonUtils::CreateErrorResponse(TEXT("Failed to duplicate blueprint"));
	}

} // namespace UnrealMCP
