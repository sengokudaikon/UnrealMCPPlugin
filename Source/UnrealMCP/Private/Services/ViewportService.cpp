#include "Services/ViewportService.h"
#include "Core/ErrorTypes.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "ImageUtils.h"
#include "LevelEditorViewport.h"
#include "Engine/Selection.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"

namespace UnrealMCP {

	auto FViewportService::FocusViewport(
		const TOptional<FString>& ActorName,
		const TOptional<FVector>& Location
	) -> FVoidResult {
		if (!GEditor) {
			return FVoidResult::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("GEditor"));
		}

		// If actor name is provided, focus on that actor
		if (ActorName.IsSet()) {
			const UWorld* World = GEditor->GetEditorWorldContext().World();
			if (!World) {
				return FVoidResult::Failure(EErrorCode::WorldNotFound);
			}

			// Find the actor
			AActor* TargetActor = nullptr;
			TArray<AActor*> AllActors;
			UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

			for (AActor* Actor : AllActors) {
				if (Actor && Actor->GetName() == ActorName.GetValue()) {
					TargetActor = Actor;
					break;
				}
			}

			if (!TargetActor) {
				return FVoidResult::Failure(EErrorCode::ActorNotFound, ActorName.GetValue());
			}

			// Select the actor and focus on it
			GEditor->SelectNone(false, true);
			GEditor->SelectActor(TargetActor, true, true);
			GEditor->MoveViewportCamerasToActor(*TargetActor, false);
		}
		// If location is provided, focus on that location
		else if (Location.IsSet()) {
			const auto ViewportClient = static_cast<FLevelEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
			if (ViewportClient) {
				ViewportClient->SetViewLocation(Location.GetValue());
			}
			else {
				return FVoidResult::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("ViewportClient"));
			}
		}
		else {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("FocusViewport"), TEXT("Either actor_name or location must be provided"));
		}

		return FVoidResult::Success();
	}

	auto FViewportService::TakeScreenshot(const FString& FilePath) -> TResult<FString> {
		if (!GEditor) {
			return TResult<FString>::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("GEditor"));
		}

		FViewport* Viewport = GEditor->GetActiveViewport();
		if (!Viewport) {
			return TResult<FString>::Failure(EErrorCode::EditorSubsystemNotFound, TEXT("ActiveViewport"));
		}

		TArray<FColor> Bitmap;
		const FIntRect ViewportRect(0, 0, Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y);

		if (Viewport->ReadPixels(Bitmap, FReadSurfaceDataFlags(), ViewportRect)) {
			TArray64<uint8> CompressedBitmap;
			FImageUtils::PNGCompressImageArray(Viewport->GetSizeXY().X,
			                                   Viewport->GetSizeXY().Y,
			                                   Bitmap,
			                                   CompressedBitmap);

			if (FFileHelper::SaveArrayToFile(CompressedBitmap, *FilePath)) {
				return TResult<FString>::Success(FilePath);
			}
			return TResult<FString>::Failure(EErrorCode::FailedToSaveAsset, FilePath, TEXT("Could not write screenshot file"));
		}

		return TResult<FString>::Failure(EErrorCode::OperationFailed, TEXT("TakeScreenshot"), TEXT("Failed to read viewport pixels"));
	}

}
