#include "Services/ActorService.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Camera/CameraActor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet/GameplayStatics.h"
#include "Editor.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "ScopedTransaction.h"
#include "Components/SceneComponent.h"

namespace UnrealMCP {

	auto FActorService::GetActorsInLevel(TArray<FString>& OutActorNames) -> FVoidResult {
		const UWorld* World = GetEditorWorld();
		if (!World) {
			return FVoidResult::Failure(TEXT("Failed to get editor world"));
		}

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

		for (const AActor* Actor : AllActors) {
			if (Actor) {
				OutActorNames.Add(Actor->GetName());
			}
		}

		return FVoidResult::Success();
	}

	auto FActorService::FindActorsByName(const FString& NamePattern, TArray<FString>& OutActorNames) -> FVoidResult {
		const UWorld* World = GetEditorWorld();
		if (!World) {
			return FVoidResult::Failure(TEXT("Failed to get editor world"));
		}

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

		for (const AActor* Actor : AllActors) {
			if (Actor && Actor->GetName().Contains(NamePattern)) {
				OutActorNames.Add(Actor->GetName());
			}
		}

		return FVoidResult::Success();
	}

	auto FActorService::SpawnActor(
		const FString& ActorClass,
		const FString& ActorName,
		const TOptional<FVector>& Location,
		const TOptional<FRotator>& Rotation
	) -> TResult<AActor*> {
		UWorld* World = GetEditorWorld();
		if (!World) {
			return TResult<AActor*>::Failure(TEXT("Failed to get editor world"));
		}

		UClass* Class = GetActorClassByName(ActorClass);
		if (!Class) {
			return TResult<AActor*>::Failure(FString::Printf(TEXT("Unknown actor class: %s"), *ActorClass));
		}

		const FVector SpawnLocation = Location.Get(FVector::ZeroVector);
		const FRotator SpawnRotation = Rotation.Get(FRotator::ZeroRotator);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = FName(*ActorName);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedActor = World->SpawnActor<AActor>(Class, SpawnLocation, SpawnRotation, SpawnParams);
		if (!SpawnedActor) {
			return TResult<AActor*>::Failure(TEXT("Failed to spawn actor"));
		}

		return TResult<AActor*>::Success(SpawnedActor);
	}

	auto FActorService::DeleteActor(const FString& ActorName) -> FVoidResult {
		AActor* Actor = FindActorByName(ActorName);
		if (!Actor) {
			return FVoidResult::Failure(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
		}

		UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		if (!EditorActorSubsystem) {
			return FVoidResult::Failure(TEXT("Failed to get EditorActorSubsystem"));
		}

		if (!EditorActorSubsystem->DestroyActor(Actor)) {
			return FVoidResult::Failure(TEXT("Failed to destroy actor"));
		}

		return FVoidResult::Success();
	}

	auto FActorService::SetActorTransform(
		const FString& ActorName,
		const TOptional<FVector>& Location,
		const TOptional<FRotator>& Rotation,
		const TOptional<FVector>& Scale
	) -> FVoidResult {
		AActor* Actor = FindActorByName(ActorName);
		if (!Actor) {
			return FVoidResult::Failure(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
		}

		// Use a scoped transaction to ensure proper editor integration
		FScopedTransaction Transaction(FText::FromString(TEXT("Set Actor Transform")));

		Actor->Modify();

		// Get the root component - this is the key for transform operations
		USceneComponent* RootComponent = Actor->GetRootComponent();
		if (!RootComponent) {
			// Create a default scene component as root for actors that don't have one
			USceneComponent* NewRoot = NewObject<USceneComponent>(Actor);
			Actor->SetRootComponent(NewRoot);
			NewRoot->RegisterComponent();
			RootComponent = NewRoot;
		}

		RootComponent->Modify();

		const FTransform CurrentTransform = RootComponent->GetRelativeTransform();
		const FVector NewLocation = Location.IsSet() ? Location.GetValue() : CurrentTransform.GetLocation();
		const FRotator NewRotation = Rotation.IsSet() ? Rotation.GetValue() : CurrentTransform.GetRotation().Rotator();
		const FVector NewScale = Scale.IsSet() ? Scale.GetValue() : CurrentTransform.GetScale3D();

		const FTransform NewTransform(NewRotation, NewLocation, NewScale);
		RootComponent->SetRelativeTransform(NewTransform, false, nullptr, ETeleportType::ResetPhysics);

		RootComponent->UpdateComponentToWorld();
		Actor->UpdateAllReplicatedComponents();

		return FVoidResult::Success();
	}

	auto FActorService::GetActorProperties(const FString& ActorName, TMap<FString, FString>& OutProperties) -> FVoidResult {
		const AActor* Actor = FindActorByName(ActorName);
		if (!Actor) {
			return FVoidResult::Failure(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
		}

		// Get basic transform properties
		const FVector Location = Actor->GetActorLocation();
		const FRotator Rotation = Actor->GetActorRotation();
		const FVector Scale = Actor->GetActorScale3D();

		OutProperties.Add(TEXT("name"), Actor->GetName());
		OutProperties.Add(TEXT("class"), Actor->GetClass()->GetName());
		OutProperties.Add(TEXT("location"), FString::Printf(TEXT("X=%f,Y=%f,Z=%f"), Location.X, Location.Y, Location.Z));
		OutProperties.Add(TEXT("rotation"), FString::Printf(TEXT("Pitch=%f,Yaw=%f,Roll=%f"), Rotation.Pitch, Rotation.Yaw, Rotation.Roll));
		OutProperties.Add(TEXT("scale"), FString::Printf(TEXT("X=%f,Y=%f,Z=%f"), Scale.X, Scale.Y, Scale.Z));

		return FVoidResult::Success();
	}

	auto FActorService::SetActorProperty(
		const FString& ActorName,
		const FString& PropertyName,
		const TSharedPtr<FJsonValue>& PropertyValue
	) -> FVoidResult {
		AActor* Actor = FindActorByName(ActorName);
		if (!Actor) {
			return FVoidResult::Failure(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
		}

		FProperty* Property = FindFProperty<FProperty>(Actor->GetClass(), *PropertyName);
		if (!Property) {
			return FVoidResult::Failure(FString::Printf(TEXT("Property not found: %s"), *PropertyName));
		}

		if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Property)) {
			bool BoolValue;
			if (!PropertyValue->TryGetBool(BoolValue)) {
				return FVoidResult::Failure(FString::Printf(TEXT("Property '%s' expects a boolean value"), *PropertyName));
			}
			BoolProp->SetPropertyValue_InContainer(Actor, BoolValue);
		}
		else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Property)) {
			double NumberValue;
			if (!PropertyValue->TryGetNumber(NumberValue)) {
				return FVoidResult::Failure(FString::Printf(TEXT("Property '%s' expects a number value"), *PropertyName));
			}
			const float Value = static_cast<float>(NumberValue);
			FloatProp->SetPropertyValue_InContainer(Actor, Value);
		}
		else if (const FIntProperty* IntProp = CastField<FIntProperty>(Property)) {
			double NumberValue;
			if (!PropertyValue->TryGetNumber(NumberValue)) {
				return FVoidResult::Failure(FString::Printf(TEXT("Property '%s' expects a number value"), *PropertyName));
			}
			const int32 Value = FMath::RoundToInt(NumberValue);
			IntProp->SetPropertyValue_InContainer(Actor, Value);
		}
		else if (const FStrProperty* StrProp = CastField<FStrProperty>(Property)) {
			FString StringValue;
			if (!PropertyValue->TryGetString(StringValue)) {
				return FVoidResult::Failure(FString::Printf(TEXT("Property '%s' expects a string value"), *PropertyName));
			}
			StrProp->SetPropertyValue_InContainer(Actor, StringValue);
		}
		else {
			return FVoidResult::Failure(FString::Printf(TEXT("Unsupported property type: %s"), *PropertyName));
		}

		return FVoidResult::Success();
	}

	UWorld* FActorService::GetEditorWorld() {
		if (GEditor) {
			return GEditor->GetEditorWorldContext().World();
		}
		return nullptr;
	}

	AActor* FActorService::FindActorByName(const FString& ActorName) {
		const UWorld* World = GetEditorWorld();
		if (!World) {
			return nullptr;
		}

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

		for (AActor* Actor : AllActors) {
			if (Actor && Actor->GetName() == ActorName) {
				return Actor;
			}
		}

		return nullptr;
	}

	UClass* FActorService::GetActorClassByName(const FString& ClassName) {
		// Map common actor class names to their UClass
		if (ClassName == TEXT("StaticMeshActor")) {
			return AStaticMeshActor::StaticClass();
		}
		if (ClassName == TEXT("DirectionalLight")) {
			return ADirectionalLight::StaticClass();
		}
		if (ClassName == TEXT("PointLight")) {
			return APointLight::StaticClass();
		}
		if (ClassName == TEXT("SpotLight")) {
			return ASpotLight::StaticClass();
		}
		if (ClassName == TEXT("CameraActor")) {
			return ACameraActor::StaticClass();
		}

		// Try to find the class by name
		UClass* Class = FindFirstObject<UClass>(*ClassName, EFindFirstObjectOptions::NativeFirst);
		if (!Class) {
			// Try with common prefixes
			const FString ClassWithPrefix = FString(TEXT("A")) + ClassName;
			Class = FindFirstObject<UClass>(*ClassWithPrefix, EFindFirstObjectOptions::NativeFirst);
		}

		return Class;
	}

} // namespace UnrealMCP
