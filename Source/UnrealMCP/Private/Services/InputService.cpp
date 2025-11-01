#include "Services/InputService.h"
#include "Core/ErrorTypes.h"
#include "Editor.h"
#include "EnhancedInputLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/World.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "UObject/SavePackage.h"
#include "HAL/PlatformFileManager.h"

namespace UnrealMCP {

	auto FInputService::CreateInputAction(const FInputActionParams& Params) -> TResult<UInputAction*> {
		if (Params.Name.IsEmpty()) {
			return TResult<UInputAction*>::Failure(EErrorCode::InvalidInput, TEXT("CreateInputAction"), TEXT("Name cannot be empty"));
		}

		// Don't add IA_ prefix if name already has it
		const FString AssetName = Params.Name.StartsWith(TEXT("IA_")) ? Params.Name : FString::Printf(TEXT("IA_%s"), *Params.Name);
		const FString PackagePath = Params.Path / AssetName;

		
		// Check if asset already exists
		const UPackage* ExistingPackage = FindPackage(nullptr, *PackagePath);
		if (ExistingPackage) {
			return TResult<UInputAction*>::Failure(
				EErrorCode::FailedToCreateAsset,
				PackagePath,
				TEXT("Input Action already exists at this path")
			);
		}

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package) {
			return TResult<UInputAction*>::Failure(EErrorCode::FailedToCreateAsset, PackagePath, TEXT("Failed to create package"));
		}

		UInputAction* InputAction = NewObject<UInputAction>(
			Package,
			*AssetName,
			RF_Public | RF_Standalone
		);
		if (!InputAction) {
			return TResult<UInputAction*>::Failure(EErrorCode::FailedToCreateAsset, AssetName, TEXT("Failed to instantiate Input Action object"));
		}

		InputAction->ValueType = static_cast<EInputActionValueType>(ParseValueType(Params.ValueType));
		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(InputAction);

		if (!SavePackage(Package, InputAction, PackagePath)) {
			return TResult<UInputAction*>::Failure(EErrorCode::FailedToSaveAsset, PackagePath);
		}

		return TResult<UInputAction*>::Success(InputAction);
	}

	auto FInputService::CreateInputMappingContext(
		const FInputMappingContextParams& Params) -> TResult<UInputMappingContext*> {
		if (Params.Name.IsEmpty()) {
			return TResult<UInputMappingContext*>::Failure(EErrorCode::InvalidInput, TEXT("CreateInputMappingContext"), TEXT("Name cannot be empty"));
		}

		// Don't add IMC_ prefix if name already has it
		const FString AssetName = Params.Name.StartsWith(TEXT("IMC_")) ? Params.Name : FString::Printf(TEXT("IMC_%s"), *Params.Name);
		const FString PackagePath = Params.Path / AssetName;

		
		// Check if asset already exists
		const UPackage* ExistingPackage = FindPackage(nullptr, *PackagePath);
		if (ExistingPackage) {
			return TResult<UInputMappingContext*>::Failure(
				EErrorCode::FailedToCreateAsset,
				PackagePath,
				TEXT("Input Mapping Context already exists at this path")
			);
		}

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package) {
			return TResult<UInputMappingContext*>::Failure(EErrorCode::FailedToCreateAsset, PackagePath, TEXT("Failed to create package"));
		}

		UInputMappingContext* MappingContext = NewObject<UInputMappingContext>(
			Package,
			*AssetName,
			RF_Public | RF_Standalone
		);
		if (!MappingContext) {
			return TResult<UInputMappingContext*>::Failure(EErrorCode::FailedToCreateAsset, AssetName, TEXT("Failed to instantiate Input Mapping Context object"));
		}

		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(MappingContext);

		if (!SavePackage(Package, MappingContext, PackagePath)) {
			return TResult<UInputMappingContext*>::Failure(EErrorCode::FailedToSaveAsset, PackagePath);
		}

		return TResult<UInputMappingContext*>::Success(MappingContext);
	}

	auto FInputService::AddMappingToContext(const FAddMappingParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("AddMappingToContext"), TEXT("Context path cannot be empty"));
		}
		if (Params.ActionPath.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("AddMappingToContext"), TEXT("Action path cannot be empty"));
		}
		if (Params.Key.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("AddMappingToContext"), TEXT("Key name cannot be empty"));
		}

		FString Error;
		UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(EErrorCode::InputMappingNotFound, Params.ContextPath, Error);
		}

		const UInputAction* InputAction = LoadInputAction(Params.ActionPath, Error);
		if (!InputAction) {
			return FVoidResult::Failure(EErrorCode::InputActionNotFound, Params.ActionPath, Error);
		}

		MappingContext->MapKey(InputAction, FKey(*Params.Key));
		MappingContext->MarkPackageDirty();

		UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(MappingContext);

		const FString PackageName = MappingContext->GetOutermost()->GetName();
		if (!SavePackage(MappingContext->GetOutermost(), MappingContext, PackageName)) {
			return FVoidResult::Failure(EErrorCode::FailedToSaveAsset, PackageName);
		}

		return FVoidResult::Success();
	}

	auto FInputService::RemoveMappingFromContext(const FAddMappingParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("RemoveMappingFromContext"), TEXT("Context path cannot be empty"));
		}
		if (Params.ActionPath.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("RemoveMappingFromContext"), TEXT("Action path cannot be empty"));
		}

		FString Error;
		UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(EErrorCode::InputMappingNotFound, Params.ContextPath, Error);
		}

		const UInputAction* InputAction = LoadInputAction(Params.ActionPath, Error);
		if (!InputAction) {
			return FVoidResult::Failure(EErrorCode::InputActionNotFound, Params.ActionPath, Error);
		}

		MappingContext->UnmapKey(InputAction, FKey());
		MappingContext->MarkPackageDirty();

		UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(MappingContext);

		const FString PackageName = MappingContext->GetOutermost()->GetName();
		if (!SavePackage(MappingContext->GetOutermost(), MappingContext, PackageName)) {
			return FVoidResult::Failure(EErrorCode::FailedToSaveAsset, PackageName);
		}

		return FVoidResult::Success();
	}

	auto FInputService::ParseValueType(const FString& ValueTypeStr) -> uint8 {
		if (ValueTypeStr == TEXT("Axis1D")) {
			return static_cast<uint8>(EInputActionValueType::Axis1D);
		}
		if (ValueTypeStr == TEXT("Axis2D")) {
			return static_cast<uint8>(EInputActionValueType::Axis2D);
		}
		if (ValueTypeStr == TEXT("Axis3D")) {
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

		// Ensure the directory exists
		const FString DirectoryPath = FPaths::GetPath(PackageFileName);
		if (!FPaths::DirectoryExists(DirectoryPath)) {
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			if (!PlatformFile.CreateDirectoryTree(*DirectoryPath)) {
				UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *DirectoryPath);
				return false;
			}
		}

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;

		return UPackage::SavePackage(Package, Asset, *PackageFileName, SaveArgs);
	}

	auto FInputService::ApplyMappingContext(const FApplyMappingContextParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("ApplyMappingContext"), TEXT("Context path cannot be empty"));
		}

		FString Error;
		const UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(EErrorCode::InputMappingNotFound, Params.ContextPath, Error);
		}

		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(Error);
		if (!Subsystem) {
			return FVoidResult::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("EnhancedInputLocalPlayerSubsystem"), Error);
		}

		Subsystem->AddMappingContext(MappingContext, Params.Priority);
		return FVoidResult::Success();
	}

	auto FInputService::RemoveMappingContext(const FRemoveMappingContextParams& Params) -> FVoidResult {
		if (Params.ContextPath.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("RemoveMappingContext"), TEXT("Context path cannot be empty"));
		}

		FString Error;
		const UInputMappingContext* MappingContext = LoadInputMappingContext(Params.ContextPath, Error);
		if (!MappingContext) {
			return FVoidResult::Failure(EErrorCode::InputMappingNotFound, Params.ContextPath, Error);
		}

		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(Error);
		if (!Subsystem) {
			return FVoidResult::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("EnhancedInputLocalPlayerSubsystem"), Error);
		}

		Subsystem->RemoveMappingContext(MappingContext);
		return FVoidResult::Success();
	}

	auto FInputService::ClearAllMappingContexts() -> FVoidResult {
		FString Error;
		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(Error);
		if (!Subsystem) {
			return FVoidResult::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("EnhancedInputLocalPlayerSubsystem"), Error);
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
			// Check if we're in PIE mode
			if (World->WorldType == EWorldType::PIE) {
				OutError = TEXT("No player controller found in PIE mode - ensure you have a Player Controller in your level");
			} else {
				OutError = TEXT("No player controller in editor (expected - enter PIE mode to test input mappings)");
			}
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
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("CreateLegacyInputMapping"), TEXT("Action name cannot be empty"));
		}

		if (Params.Key.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("CreateLegacyInputMapping"), TEXT("Key cannot be empty"));
		}

		// Get the input settings
		UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();
		if (!InputSettings) {
			return FVoidResult::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("UInputSettings"));
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

	auto FInputService::CreatePlayerControllerInEditor() -> FVoidResult {
		UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
		if (!World) {
			return FVoidResult::Failure(EErrorCode::OperationFailed, TEXT("CreatePlayerControllerInEditor"), TEXT("Failed to get editor world"));
		}

		// Check if we already have a PlayerController
		const APlayerController* ExistingController = World->GetFirstPlayerController();
		if (ExistingController) {
			return FVoidResult::Success(); // PlayerController already exists
		}

		// Try to spawn a PlayerController
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = nullptr;
		SpawnParams.Instigator = nullptr;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.bDeferConstruction = false;

		// Get the default PlayerController class from the game mode or use APlayerController as fallback
		TSubclassOf<APlayerController> ControllerClass = APlayerController::StaticClass();
		if (World->GetAuthGameMode()) {
			ControllerClass = World->GetAuthGameMode()->PlayerControllerClass;
		}

		APlayerController* NewController = World->SpawnActor<APlayerController>(ControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (!NewController) {
			return FVoidResult::Failure(EErrorCode::OperationFailed, TEXT("CreatePlayerControllerInEditor"), TEXT("Failed to spawn PlayerController"));
		}

		// Initialize the controller
		if (World->GetGameInstance()) {
			// Create a new player and assign it to this controller
			UGameplayStatics::CreatePlayer(World, 0, true);
			NewController->SetPlayer(0); // Set as player 0
		}

		return FVoidResult::Success();
	}

}
