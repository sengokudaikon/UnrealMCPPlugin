#include "Services/InputService.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Misc/Paths.h"
#include "Editor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"

namespace UnrealMCP {

	auto FInputService::CreateInputAction(const FInputActionParams& Params) -> TResult<UInputAction*> {
		if (Params.Name.IsEmpty()) {
			return TResult<UInputAction*>::Failure(TEXT("Input action name cannot be empty"));
		}

		const FString AssetName = FString::Printf(TEXT("IA_%s"), *Params.Name);
		const FString PackagePath = Params.Path / AssetName;

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package) {
			return TResult<UInputAction*>::Failure(TEXT("Failed to create package for input action"));
		}

		UInputAction* InputAction = NewObject<UInputAction>(
			Package,
			*AssetName,
			RF_Public | RF_Standalone
		);
		if (!InputAction) {
			return TResult<UInputAction*>::Failure(TEXT("Failed to instantiate Input Action object"));
		}

		InputAction->ValueType = static_cast<EInputActionValueType>(ParseValueType(Params.ValueType));
		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(InputAction);

		if (!SavePackage(Package, InputAction, PackagePath)) {
			return TResult<UInputAction*>::Failure(TEXT("Failed to save Input Action asset to disk"));
		}

		return TResult<UInputAction*>::Success(InputAction);
	}

	auto FInputService::CreateInputMappingContext(
		const FInputMappingContextParams& Params) -> TResult<UInputMappingContext*> {
		if (Params.Name.IsEmpty()) {
			return TResult<UInputMappingContext*>::Failure(TEXT("Mapping context name cannot be empty"));
		}

		const FString AssetName = FString::Printf(TEXT("IMC_%s"), *Params.Name);
		const FString PackagePath = Params.Path / AssetName;

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package) {
			return TResult<UInputMappingContext*>::Failure(TEXT("Failed to create package for mapping context"));
		}

		UInputMappingContext* MappingContext = NewObject<UInputMappingContext>(
			Package,
			*AssetName,
			RF_Public | RF_Standalone
		);
		if (!MappingContext) {
			return TResult<UInputMappingContext*>::Failure(TEXT("Failed to instantiate Input Mapping Context object"));
		}

		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(MappingContext);

		if (!SavePackage(Package, MappingContext, PackagePath)) {
			return TResult<UInputMappingContext*>::Failure(TEXT("Failed to save Input Mapping Context asset to disk"));
		}

		return TResult<UInputMappingContext*>::Success(MappingContext);
	}

	auto FInputService::AddMappingToContext(const FAddMappingParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Input mapping context path cannot be empty"));
		}
		if (Params.ActionPath.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Input action path cannot be empty"));
		}
		if (Params.Key.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Key name cannot be empty"));
		}

		FString Error;
		UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(Error);
		}

		const UInputAction* InputAction = LoadInputAction(Params.ActionPath, Error);
		if (!InputAction) {
			return FVoidResult::Failure(Error);
		}

		MappingContext->MapKey(InputAction, FKey(*Params.Key));
		MappingContext->MarkPackageDirty();

		UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(MappingContext);

		const FString PackageName = MappingContext->GetOutermost()->GetName();
		if (!SavePackage(MappingContext->GetOutermost(), MappingContext, PackageName)) {
			return FVoidResult::Failure(TEXT("Failed to save Input Mapping Context after adding mapping"));
		}

		return FVoidResult::Success();
	}

	auto FInputService::RemoveMappingFromContext(const FAddMappingParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Input mapping context path cannot be empty"));
		}
		if (Params.ActionPath.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Input action path cannot be empty"));
		}

		FString Error;
		UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(Error);
		}

		const UInputAction* InputAction = LoadInputAction(Params.ActionPath, Error);
		if (!InputAction) {
			return FVoidResult::Failure(Error);
		}

		MappingContext->UnmapKey(InputAction, FKey());
		MappingContext->MarkPackageDirty();

		UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(MappingContext);

		const FString PackageName = MappingContext->GetOutermost()->GetName();
		if (!SavePackage(MappingContext->GetOutermost(), MappingContext, PackageName)) {
			return FVoidResult::Failure(TEXT("Failed to save Input Mapping Context after removing mapping"));
		}

		return FVoidResult::Success();
	}

	auto FInputService::ParseValueType(const FString& ValueTypeStr) -> uint8 {
		if (ValueTypeStr == TEXT("Axis1D")) {
			return static_cast<uint8>(EInputActionValueType::Axis1D);
		}
		else if (ValueTypeStr == TEXT("Axis2D")) {
			return static_cast<uint8>(EInputActionValueType::Axis2D);
		}
		else if (ValueTypeStr == TEXT("Axis3D")) {
			return static_cast<uint8>(EInputActionValueType::Axis3D);
		}
		return static_cast<uint8>(EInputActionValueType::Boolean);
	}

	auto FInputService::LoadInputAction(const FString& AssetPath, FString& OutError) -> UInputAction* {
		if (AssetPath.IsEmpty()) {
			OutError = TEXT("Input action path cannot be empty");
			return nullptr;
		}

		UInputAction* InputAction = LoadObject<UInputAction>(nullptr, *AssetPath);
		if (!InputAction) {
			OutError = FString::Printf(TEXT("Failed to load Input Action: %s"), *AssetPath);
		}
		return InputAction;
	}

	auto FInputService::LoadInputMappingContext(const FString& AssetPath, FString& OutError) -> UInputMappingContext* {
		if (AssetPath.IsEmpty()) {
			OutError = TEXT("Input mapping context path cannot be empty");
			return nullptr;
		}

		UInputMappingContext* MappingContext = LoadObject<UInputMappingContext>(nullptr, *AssetPath);
		if (!MappingContext) {
			OutError = FString::Printf(TEXT("Failed to load Input Mapping Context: %s"), *AssetPath);
		}
		return MappingContext;
	}

	auto FInputService::SavePackage(UPackage* Package, UObject* Asset, const FString& PackagePath) -> bool {
		if (!Package || !Asset) {
			return false;
		}

		const FString PackageFileName = FPackageName::LongPackageNameToFilename(
			PackagePath,
			FPackageName::GetAssetPackageExtension()
		);

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;

		return UPackage::SavePackage(Package, Asset, *PackageFileName, SaveArgs);
	}

	auto FInputService::ApplyMappingContext(const FApplyMappingContextParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Input mapping context path cannot be empty"));
		}

		FString Error;
		const UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(Error);
		}

		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(Error);
		if (!Subsystem) {
			return FVoidResult::Failure(Error);
		}

		Subsystem->AddMappingContext(MappingContext, Params.Priority);
		return FVoidResult::Success();
	}

	auto FInputService::RemoveMappingContext(const FRemoveMappingContextParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Input mapping context path cannot be empty"));
		}

		FString Error;
		const UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(Error);
		}

		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(Error);
		if (!Subsystem) {
			return FVoidResult::Failure(Error);
		}

		Subsystem->RemoveMappingContext(MappingContext);
		return FVoidResult::Success();
	}

	auto FInputService::ClearAllMappingContexts() -> FVoidResult {
		FString Error;
		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(Error);
		if (!Subsystem) {
			return FVoidResult::Failure(Error);
		}

		Subsystem->ClearAllMappings();
		return FVoidResult::Success();
	}

	auto FInputService::GetInputSubsystem(FString& OutError) -> UEnhancedInputLocalPlayerSubsystem* {
		const UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
		if (!World) {
			OutError = TEXT("Failed to get world");
			return nullptr;
		}

		const APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!PlayerController) {
			OutError = TEXT("No player controller found");
			return nullptr;
		}

		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer());
		if (!Subsystem) {
			OutError = TEXT("Failed to get Enhanced Input Subsystem");
			return nullptr;
		}

		return Subsystem;
	}

	auto FInputService::CreateLegacyInputMapping(const FLegacyInputMappingParams& Params) -> FVoidResult {
		if (Params.ActionName.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Action name cannot be empty"));
		}

		if (Params.Key.IsEmpty()) {
			return FVoidResult::Failure(TEXT("Key cannot be empty"));
		}

		// Get the input settings
		UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();
		if (!InputSettings) {
			return FVoidResult::Failure(TEXT("Failed to get input settings"));
		}

		// Create the input action mapping
		FInputActionKeyMapping ActionMapping;
		ActionMapping.ActionName = FName(*Params.ActionName);
		ActionMapping.Key = FKey(*Params.Key);
		ActionMapping.bShift = Params.bShift;
		ActionMapping.bCtrl = Params.bCtrl;
		ActionMapping.bAlt = Params.bAlt;
		ActionMapping.bCmd = Params.bCmd;

		// Add the mapping
		InputSettings->AddActionMapping(ActionMapping);

		// Save the configuration
		InputSettings->SaveConfig();
		return FVoidResult::Success();
	}

} // namespace UnrealMCP
