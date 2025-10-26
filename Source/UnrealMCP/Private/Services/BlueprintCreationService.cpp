#include "Services/BlueprintCreationService.h"
#include "Commands/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Factories/BlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "EditorAssetLibrary.h"
#include "Kismet2/KismetEditorUtilities.h"

namespace UnrealMCP
{
	auto FBlueprintCreationService::CreateBlueprint(const FBlueprintCreationParams& Params) -> TResult<UBlueprint*>
	{
		if (Params.Name.IsEmpty())
		{
			return TResult<UBlueprint*>::Failure(TEXT("Blueprint name cannot be empty"));
		}

		const FString FullAssetPath = Params.PackagePath + Params.Name;

		if (UEditorAssetLibrary::DoesAssetExist(FullAssetPath))
		{
			return TResult<UBlueprint*>::Failure(
				FString::Printf(TEXT("Blueprint already exists at: %s"), *FullAssetPath)
			);
		}

		UClass* ParentClass = ResolveParentClass(Params.ParentClass);
		if (!ParentClass)
		{
			return TResult<UBlueprint*>::Failure(
				FString::Printf(TEXT("Unable to resolve parent class: %s"), *Params.ParentClass)
			);
		}

		UBlueprintFactory* Factory = NewObject<UBlueprintFactory>();
		if (!Factory)
		{
			return TResult<UBlueprint*>::Failure(TEXT("Failed to create blueprint factory"));
		}

		Factory->ParentClass = ParentClass;

		UPackage* Package = CreatePackage(*FullAssetPath);
		if (!Package)
		{
			return TResult<UBlueprint*>::Failure(
				FString::Printf(TEXT("Failed to create package: %s"), *FullAssetPath)
			);
		}

		UBlueprint* NewBlueprint = Cast<UBlueprint>(
			Factory->FactoryCreateNew(
				UBlueprint::StaticClass(),
				Package,
				*Params.Name,
				RF_Standalone | RF_Public,
				nullptr,
				GWarn
			)
		);

		if (!NewBlueprint)
		{
			return TResult<UBlueprint*>::Failure(TEXT("Failed to create blueprint asset"));
		}

		FAssetRegistryModule::AssetCreated(NewBlueprint);
		Package->MarkPackageDirty();

		UE_LOG(
			LogTemp,
			Display,
			TEXT("BlueprintCreationService: Created blueprint '%s' with parent class '%s' at '%s'"),
			*Params.Name,
			*ParentClass->GetName(),
			*FullAssetPath
		);

		return TResult<UBlueprint*>::Success(NewBlueprint);
	}

	auto FBlueprintCreationService::CompileBlueprint(const FString& BlueprintName) -> FVoidResult
	{
		if (BlueprintName.IsEmpty())
		{
			return FVoidResult::Failure(TEXT("Blueprint name cannot be empty"));
		}

		UBlueprint* Blueprint = FCommonUtils::FindBlueprint(BlueprintName);
		if (!Blueprint)
		{
			return FVoidResult::Failure(FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName));
		}

		FKismetEditorUtilities::CompileBlueprint(Blueprint);

		UE_LOG(
			LogTemp,
			Display,
			TEXT("BlueprintCreationService: Compiled blueprint '%s'"),
			*BlueprintName
		);

		return FVoidResult::Success();
	}

	auto FBlueprintCreationService::ResolveParentClass(const FString& ParentClassName) -> UClass*
	{
		if (ParentClassName.IsEmpty() || ParentClassName == TEXT("Actor"))
		{
			return AActor::StaticClass();
		}

		if (ParentClassName == TEXT("Pawn"))
		{
			return APawn::StaticClass();
		}

		if (ParentClassName == TEXT("Character"))
		{
			return ACharacter::StaticClass();
		}

		FString NormalizedClassName = ParentClassName;

		// Add 'A' prefix if not present (Unreal Engine convention for Actor-derived classes)
		if (!NormalizedClassName.StartsWith(TEXT("A")))
		{
			NormalizedClassName = TEXT("A") + NormalizedClassName;
		}

		// Handle common cases directly
		if (NormalizedClassName == TEXT("AActor"))
		{
			return AActor::StaticClass();
		}

		if (NormalizedClassName == TEXT("APawn"))
		{
			return APawn::StaticClass();
		}

		if (NormalizedClassName == TEXT("ACharacter"))
		{
			return ACharacter::StaticClass();
		}

		// Try to load from Engine module
		const FString EngineClassPath = FString::Printf(TEXT("/Script/Engine.%s"), *NormalizedClassName);
		UClass* FoundClass = LoadClass<AActor>(nullptr, *EngineClassPath);

		if (FoundClass)
		{
			UE_LOG(
				LogTemp,
				Verbose,
				TEXT("BlueprintCreationService: Resolved parent class '%s' from Engine module"),
				*NormalizedClassName
			);
			return FoundClass;
		}

		// Try to load from Game module
		const FString GameClassPath = FString::Printf(TEXT("/Script/Game.%s"), *NormalizedClassName);
		FoundClass = LoadClass<AActor>(nullptr, *GameClassPath);

		if (FoundClass)
		{
			UE_LOG(
				LogTemp,
				Verbose,
				TEXT("BlueprintCreationService: Resolved parent class '%s' from Game module"),
				*NormalizedClassName
			);
			return FoundClass;
		}

		// Fallback to AActor with warning
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("BlueprintCreationService: Could not resolve parent class '%s' - defaulting to AActor"),
			*ParentClassName
		);

		return AActor::StaticClass();
	}
} // namespace UnrealMCP
