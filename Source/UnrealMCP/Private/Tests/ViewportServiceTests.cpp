/**
 * Functional tests for ViewportService
 *
 * These tests verify the actual behavior of viewport operations:
 * - Focusing on actors
 * - Focusing on locations
 * - Taking screenshots
 *
 * Tests run in the Unreal Editor with real world context.
 */

#include "Services/ViewportService.h"
#include "Misc/AutomationTest.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "Editor.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "Engine/Selection.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FViewportServiceFocusOnActorTest,
	"UnrealMCP.Viewport.FocusOnActor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FViewportServiceFocusOnActorTest::RunTest(const FString& Parameters)
{
	// Test: Focus viewport on a spawned actor

	UWorld* World = GEditor->GetEditorWorldContext().World();
	TestNotNull(TEXT("Editor world should be available"), World);
	if (!World) return false;

	// Spawn a test actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(TEXT("ViewportTestActor"));
	AActor* TestActor = World->SpawnActor<AActor>(
		AActor::StaticClass(),
		FVector(100.0f, 200.0f, 300.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);
	TestNotNull(TEXT("Test actor should spawn successfully"), TestActor);
	if (!TestActor) return false;

	// Set a recognizable name so we can find it
	TestActor->Rename(TEXT("ViewportTestActor"));

	// Focus on the actor using the service
	const UnrealMCP::FVoidResult Result = UnrealMCP::FViewportService::FocusViewport(
		TOptional<FString>(TEXT("ViewportTestActor")),
		TOptional<FVector>()
	);

	// Verify success
	TestTrue(TEXT("FocusViewport should succeed for valid actor"), Result.IsSuccess());

	// Verify the actor is selected
	const USelection* Selection = GEditor->GetSelectedActors();
	TestTrue(TEXT("Actor should be selected after focus"), Selection && Selection->IsSelected(TestActor));

	// Cleanup
	World->DestroyActor(TestActor);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FViewportServiceFocusOnLocationTest,
	"UnrealMCP.Viewport.FocusOnLocation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FViewportServiceFocusOnLocationTest::RunTest(const FString& Parameters)
{
	// Test: Focus viewport on a specific location

	const FVector TargetLocation(500.0f, 1000.0f, 250.0f);

	// Focus on the location using the service
	const UnrealMCP::FVoidResult Result = UnrealMCP::FViewportService::FocusViewport(
		TOptional<FString>(),
		TOptional<FVector>(TargetLocation)
	);

	// Verify success
	TestTrue(TEXT("FocusViewport should succeed for valid location"), Result.IsSuccess());

	// Note: We can't easily verify the camera moved without more complex viewport introspection
	// But we verified the operation succeeded without error

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FViewportServiceFocusOnInvalidActorTest,
	"UnrealMCP.Viewport.FocusOnInvalidActor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FViewportServiceFocusOnInvalidActorTest::RunTest(const FString& Parameters)
{
	// Test: Focus on non-existent actor should fail gracefully

	const UnrealMCP::FVoidResult Result = UnrealMCP::FViewportService::FocusViewport(
		TOptional<FString>(TEXT("NonExistentActor_XYZ123")),
		TOptional<FVector>()
	);

	// Verify failure
	TestTrue(TEXT("FocusViewport should fail for non-existent actor"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention 'not found'"),
		Result.GetError().Contains(TEXT("not found")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FViewportServiceFocusWithoutParametersTest,
	"UnrealMCP.Viewport.FocusWithoutParameters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FViewportServiceFocusWithoutParametersTest::RunTest(const FString& Parameters)
{
	// Test: Focus without actor name or location should fail

	const UnrealMCP::FVoidResult Result = UnrealMCP::FViewportService::FocusViewport(
		TOptional<FString>(),
		TOptional<FVector>()
	);

	// Verify failure
	TestTrue(TEXT("FocusViewport should fail when neither actor nor location provided"),
		Result.IsFailure());
	TestTrue(TEXT("Error message should mention required parameters"),
		Result.GetError().Contains(TEXT("must be provided")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FViewportServiceTakeScreenshotTest,
	"UnrealMCP.Viewport.TakeScreenshot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FViewportServiceTakeScreenshotTest::RunTest(const FString& Parameters)
{
	// Test: Take a screenshot and verify file is created

	// Use temp directory for test screenshot
	const FString ScreenshotPath = FPaths::ProjectSavedDir() / TEXT("Tests") / TEXT("test_screenshot.png");

	// Ensure directory exists
	const FString DirectoryPath = FPaths::GetPath(ScreenshotPath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*DirectoryPath))
	{
		PlatformFile.CreateDirectoryTree(*DirectoryPath);
	}

	// Delete any existing test screenshot
	if (PlatformFile.FileExists(*ScreenshotPath))
	{
		PlatformFile.DeleteFile(*ScreenshotPath);
	}

	// Take screenshot
	UnrealMCP::TResult<FString> Result = UnrealMCP::FViewportService::TakeScreenshot(ScreenshotPath);

	// Verify success
	TestTrue(TEXT("TakeScreenshot should succeed"), Result.IsSuccess());
	if (Result.IsSuccess())
	{
		TestEqual(TEXT("Returned path should match requested path"),
			Result.GetValue(), ScreenshotPath);
	}

	// Verify file was created
	TestTrue(TEXT("Screenshot file should exist on disk"),
		PlatformFile.FileExists(*ScreenshotPath));

	// Verify file has content
	if (PlatformFile.FileExists(*ScreenshotPath))
	{
		const int64 FileSize = PlatformFile.FileSize(*ScreenshotPath);
		TestTrue(TEXT("Screenshot file should have content (size > 0)"), FileSize > 0);

		// PNG files start with specific header bytes
		TArray<uint8> FileHeader;
		if (FFileHelper::LoadFileToArray(FileHeader, *ScreenshotPath))
		{
			if (FileHeader.Num() >= 8)
			{
				// PNG signature: 137 80 78 71 13 10 26 10
				TestTrue(TEXT("Screenshot should be valid PNG format"),
					FileHeader[0] == 137 && FileHeader[1] == 80 &&
					FileHeader[2] == 78 && FileHeader[3] == 71);
			}
		}
	}

	// Cleanup
	if (PlatformFile.FileExists(*ScreenshotPath))
	{
		PlatformFile.DeleteFile(*ScreenshotPath);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FViewportServiceScreenshotInvalidPathTest,
	"UnrealMCP.Viewport.ScreenshotInvalidPath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FViewportServiceScreenshotInvalidPathTest::RunTest(const FString& Parameters)
{
	// Test: Taking screenshot to invalid/unwritable path should fail gracefully

	// Use an invalid path (drive that likely doesn't exist or invalid characters)
	const FString InvalidPath = TEXT("Z:/InvalidDrive/NonExistent/Path/test.png");

	const UnrealMCP::TResult<FString> Result = UnrealMCP::FViewportService::TakeScreenshot(InvalidPath);

	// Verify failure
	TestTrue(TEXT("TakeScreenshot should fail for invalid path"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention save failure"),
		Result.GetError().Contains(TEXT("Failed")));

	return true;
}
