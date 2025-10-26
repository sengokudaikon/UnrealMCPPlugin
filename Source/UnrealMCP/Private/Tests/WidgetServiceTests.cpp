/**
 * Functional tests for WidgetService
 *
 * These tests verify the actual behavior of UMG widget operations:
 * - Creating widget blueprints
 * - Adding text blocks to widgets
 * - Adding buttons to widgets
 * - Binding widget events
 * - Setting text block bindings
 * - Getting widget classes
 *
 * Tests run in the Unreal Editor with real world context.
 */

#include "Services/WidgetService.h"
#include "Core/MCPTypes.h"
#include "Misc/AutomationTest.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "EditorAssetLibrary.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceCreateWidgetTest,
	"UnrealMCP.Widget.CreateWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceCreateWidgetTest::RunTest(const FString& Parameters)
{
	// Test: Create a basic widget blueprint

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/TestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	UnrealMCP::FWidgetCreationParams Params;
	Params.Name = TEXT("TestWidget");
	Params.ParentClass = TEXT("UserWidget");
	Params.PackagePath = TEXT("/Game/UI");

	UnrealMCP::TResult<UWidgetBlueprint*> Result = UnrealMCP::FWidgetService::CreateWidget(Params);

	// Verify success
	if (Result.IsFailure())
	{
		AddError(FString::Printf(TEXT("CreateWidget failed with error: %s"), *Result.GetError()));
	}
	TestTrue(TEXT("CreateWidget should succeed"), Result.IsSuccess());
	const UWidgetBlueprint* WidgetBlueprint = Result.GetValue();
	TestTrue(TEXT("WidgetBlueprint should not be null"), WidgetBlueprint != nullptr);
	if (WidgetBlueprint)
	{
		TestEqual(TEXT("Widget name should match"), WidgetBlueprint->GetFName().ToString(), TEXT("TestWidget"));
		TestTrue(TEXT("Widget should have a WidgetTree"), WidgetBlueprint->WidgetTree != nullptr);
		TestTrue(TEXT("Widget should have a root widget"), WidgetBlueprint->WidgetTree->RootWidget != nullptr);
		TestTrue(TEXT("Root widget should be a Canvas Panel"),
			WidgetBlueprint->WidgetTree->RootWidget->IsA<UCanvasPanel>());
	}

	// Cleanup - delete the created asset
	if (WidgetBlueprint)
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceCreateDuplicateWidgetTest,
	"UnrealMCP.Widget.CreateDuplicateWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceCreateDuplicateWidgetTest::RunTest(const FString& Parameters)
{
	// Test: Creating a widget with the same name should fail

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/DuplicateTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	UnrealMCP::FWidgetCreationParams Params;
	Params.Name = TEXT("DuplicateTestWidget");
	Params.ParentClass = TEXT("UserWidget");
	Params.PackagePath = TEXT("/Game/UI");

	// Create the first widget
	const UnrealMCP::TResult<UWidgetBlueprint*> FirstResult = UnrealMCP::FWidgetService::CreateWidget(Params);
	TestTrue(TEXT("First CreateWidget should succeed"), FirstResult.IsSuccess());

	// Try to create a second widget with the same name
	const UnrealMCP::TResult<UWidgetBlueprint*> SecondResult = UnrealMCP::FWidgetService::CreateWidget(Params);

	// Verify failure
	TestTrue(TEXT("Second CreateWidget should fail"), SecondResult.IsFailure());
	TestTrue(TEXT("Error message should mention already exists"),
		SecondResult.GetError().Contains(TEXT("already exists")));

	// Cleanup
	if (FirstResult.IsSuccess())
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceCreateWidgetWithInvalidParentTest,
	"UnrealMCP.Widget.CreateWidgetWithInvalidParent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceCreateWidgetWithInvalidParentTest::RunTest(const FString& Parameters)
{
	// Test: Creating widget with invalid parent class should still work (defaults to UserWidget)

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/InvalidParentTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	UnrealMCP::FWidgetCreationParams Params;
	Params.Name = TEXT("InvalidParentTestWidget");
	Params.ParentClass = TEXT("NonExistentClass_XYZ123");
	Params.PackagePath = TEXT("/Game/UI");

	const UnrealMCP::TResult<UWidgetBlueprint*> Result = UnrealMCP::FWidgetService::CreateWidget(Params);

	// This should actually succeed because the CreateWidget function defaults to UserWidget
	// and doesn't validate the parent class name
	TestTrue(TEXT("CreateWidget should succeed even with invalid parent"), Result.IsSuccess());

	// Cleanup
	if (Result.IsSuccess())
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceAddTextBlockTest,
	"UnrealMCP.Widget.AddTextBlock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceAddTextBlockTest::RunTest(const FString& Parameters)
{
	// Test: Add a text block to an existing widget

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/TextBlockTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	// First create a widget
	UnrealMCP::FWidgetCreationParams CreateParams;
	CreateParams.Name = TEXT("TextBlockTestWidget");
	CreateParams.PackagePath = TEXT("/Game/UI");

	UnrealMCP::TResult<UWidgetBlueprint*> CreateResult = UnrealMCP::FWidgetService::CreateWidget(CreateParams);
	TestTrue(TEXT("CreateWidget should succeed"), CreateResult.IsSuccess());
	if (!CreateResult.IsSuccess()) return false;

	UWidgetBlueprint* WidgetBlueprint = CreateResult.GetValue();

	// Add a text block
	UnrealMCP::FTextBlockParams TextParams;
	TextParams.WidgetName = TEXT("TextBlockTestWidget");
	TextParams.TextBlockName = TEXT("TestTextBlock");
	TextParams.Text = TEXT("Hello World");
	TextParams.Position = FVector2D(100.0f, 50.0f);
	TextParams.Size = FVector2D(200.0f, 30.0f);
	TextParams.FontSize = 16;
	TextParams.Color = FLinearColor::Red;

	UnrealMCP::TResult<UTextBlock*> TextResult = UnrealMCP::FWidgetService::AddTextBlock(TextParams);

	// Verify success
	TestTrue(TEXT("AddTextBlock should succeed"), TextResult.IsSuccess());
	const UTextBlock* TextBlock = TextResult.GetValue();
	TestTrue(TEXT("TextBlock should not be null"), TextBlock != nullptr);
	if (TextBlock)
	{
		TestEqual(TEXT("TextBlock name should match"), TextBlock->GetFName().ToString(), TEXT("TestTextBlock"));
		TestEqual(TEXT("TextBlock text should match"), TextBlock->GetText().ToString(), TEXT("Hello World"));
		TestEqual(TEXT("TextBlock font size should be 16"), (int32)TextBlock->GetFont().Size, 16);
	}

	// Cleanup
	UEditorAssetLibrary::DeleteAsset(AssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceAddTextBlockToInvalidWidgetTest,
	"UnrealMCP.Widget.AddTextBlockToInvalidWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceAddTextBlockToInvalidWidgetTest::RunTest(const FString& Parameters)
{
	// Test: Adding text block to non-existent widget should fail

	// Suppress expected errors from asset loading
	AddExpectedError(TEXT("LoadAsset failed"), EAutomationExpectedErrorFlags::Contains, 1);

	UnrealMCP::FTextBlockParams TextParams;
	TextParams.WidgetName = TEXT("NonExistentWidget_XYZ123");
	TextParams.TextBlockName = TEXT("TestTextBlock");
	TextParams.Text = TEXT("Hello World");

	const UnrealMCP::TResult<UTextBlock*> Result = UnrealMCP::FWidgetService::AddTextBlock(TextParams);

	// Verify failure
	TestTrue(TEXT("AddTextBlock should fail for non-existent widget"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceAddButtonTest,
	"UnrealMCP.Widget.AddButton",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceAddButtonTest::RunTest(const FString& Parameters)
{
	// Test: Add a button to an existing widget

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/ButtonTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	// First create a widget
	UnrealMCP::FWidgetCreationParams CreateParams;
	CreateParams.Name = TEXT("ButtonTestWidget");
	CreateParams.PackagePath = TEXT("/Game/UI");

	UnrealMCP::TResult<UWidgetBlueprint*> CreateResult = UnrealMCP::FWidgetService::CreateWidget(CreateParams);
	TestTrue(TEXT("CreateWidget should succeed"), CreateResult.IsSuccess());
	if (!CreateResult.IsSuccess()) return false;

	UWidgetBlueprint* WidgetBlueprint = CreateResult.GetValue();

	// Add a button
	UnrealMCP::FButtonParams ButtonParams;
	ButtonParams.WidgetName = TEXT("ButtonTestWidget");
	ButtonParams.ButtonName = TEXT("TestButton");
	ButtonParams.Text = TEXT("Click Me");
	ButtonParams.Position = FVector2D(50.0f, 100.0f);
	ButtonParams.Size = FVector2D(150.0f, 40.0f);
	ButtonParams.FontSize = 14;
	ButtonParams.TextColor = FLinearColor::White;
	ButtonParams.BackgroundColor = FLinearColor::Blue;

	UnrealMCP::TResult<UButton*> ButtonResult = UnrealMCP::FWidgetService::AddButton(ButtonParams);

	// Verify success
	TestTrue(TEXT("AddButton should succeed"), ButtonResult.IsSuccess());
	const UButton* Button = ButtonResult.GetValue();
	TestTrue(TEXT("Button should not be null"), Button != nullptr);
	if (Button)
	{
		TestEqual(TEXT("Button name should match"), Button->GetFName().ToString(), TEXT("TestButton"));
	}

	// Cleanup
	UEditorAssetLibrary::DeleteAsset(AssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceAddButtonToInvalidWidgetTest,
	"UnrealMCP.Widget.AddButtonToInvalidWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceAddButtonToInvalidWidgetTest::RunTest(const FString& Parameters)
{
	// Test: Adding button to non-existent widget should fail

	// Suppress expected errors from asset loading
	AddExpectedError(TEXT("LoadAsset failed"), EAutomationExpectedErrorFlags::Contains, 1);

	UnrealMCP::FButtonParams ButtonParams;
	ButtonParams.WidgetName = TEXT("NonExistentWidget_XYZ123");
	ButtonParams.ButtonName = TEXT("TestButton");
	ButtonParams.Text = TEXT("Click Me");

	const UnrealMCP::TResult<UButton*> Result = UnrealMCP::FWidgetService::AddButton(ButtonParams);

	// Verify failure
	TestTrue(TEXT("AddButton should fail for non-existent widget"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceBindWidgetEventTest,
	"UnrealMCP.Widget.BindWidgetEvent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceBindWidgetEventTest::RunTest(const FString& Parameters)
{
	// Test: Bind an event to a widget component

	// Cleanup any existing widget from previous failed tests
	FString AssetPath = TEXT("/Game/UI/EventBindTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	// First create a widget with a button
	UnrealMCP::FWidgetCreationParams CreateParams;
	CreateParams.Name = TEXT("EventBindTestWidget");
	CreateParams.PackagePath = TEXT("/Game/UI");

	UnrealMCP::TResult<UWidgetBlueprint*> CreateResult = UnrealMCP::FWidgetService::CreateWidget(CreateParams);
	TestTrue(TEXT("CreateWidget should succeed"), CreateResult.IsSuccess());
	if (!CreateResult.IsSuccess()) return false;

	// Add a button to bind the event to
	UnrealMCP::FButtonParams ButtonParams;
	ButtonParams.WidgetName = TEXT("EventBindTestWidget");
	ButtonParams.ButtonName = TEXT("EventTestButton");
	ButtonParams.Text = TEXT("Click Me");

	UnrealMCP::TResult<UButton*> ButtonResult = UnrealMCP::FWidgetService::AddButton(ButtonParams);
	TestTrue(TEXT("AddButton should succeed"), ButtonResult.IsSuccess());
	if (!ButtonResult.IsSuccess())
	{
		// Cleanup before returning
		UEditorAssetLibrary::DeleteAsset(AssetPath);
		return false;
	}

	// Bind the event
	UnrealMCP::FWidgetEventBindingParams EventParams;
	EventParams.WidgetName = TEXT("EventBindTestWidget");
	EventParams.WidgetComponentName = TEXT("EventTestButton");
	EventParams.EventName = TEXT("OnClicked");
	EventParams.FunctionName = TEXT("OnButtonClicked");

	UnrealMCP::FVoidResult EventResult = UnrealMCP::FWidgetService::BindWidgetEvent(EventParams);

	// Verify success
	TestTrue(TEXT("BindWidgetEvent should succeed"), EventResult.IsSuccess());

	// Cleanup
	UEditorAssetLibrary::DeleteAsset(AssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceBindWidgetEventInvalidWidgetTest,
	"UnrealMCP.Widget.BindWidgetEventInvalidWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceBindWidgetEventInvalidWidgetTest::RunTest(const FString& Parameters)
{
	// Test: Binding event to non-existent widget should fail

	// Suppress expected errors from asset loading
	AddExpectedError(TEXT("LoadAsset failed"), EAutomationExpectedErrorFlags::Contains, 1);

	UnrealMCP::FWidgetEventBindingParams EventParams;
	EventParams.WidgetName = TEXT("NonExistentWidget_XYZ123");
	EventParams.WidgetComponentName = TEXT("SomeButton");
	EventParams.EventName = TEXT("OnClicked");
	EventParams.FunctionName = TEXT("OnButtonClicked");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FWidgetService::BindWidgetEvent(EventParams);

	// Verify failure
	TestTrue(TEXT("BindWidgetEvent should fail for non-existent widget"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceBindWidgetEventInvalidComponentTest,
	"UnrealMCP.Widget.BindWidgetEventInvalidComponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceBindWidgetEventInvalidComponentTest::RunTest(const FString& Parameters)
{
	// Test: Binding event to non-existent component should fail

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/InvalidComponentTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	// First create a widget
	UnrealMCP::FWidgetCreationParams CreateParams;
	CreateParams.Name = TEXT("InvalidComponentTestWidget");
	CreateParams.PackagePath = TEXT("/Game/UI");

	const UnrealMCP::TResult<UWidgetBlueprint*> CreateResult = UnrealMCP::FWidgetService::CreateWidget(CreateParams);
	TestTrue(TEXT("CreateWidget should succeed"), CreateResult.IsSuccess());
	if (!CreateResult.IsSuccess()) return false;

	// Try to bind to a non-existent component
	UnrealMCP::FWidgetEventBindingParams EventParams;
	EventParams.WidgetName = TEXT("InvalidComponentTestWidget");
	EventParams.WidgetComponentName = TEXT("NonExistentButton_XYZ123");
	EventParams.EventName = TEXT("OnClicked");
	EventParams.FunctionName = TEXT("OnButtonClicked");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FWidgetService::BindWidgetEvent(EventParams);

	// Verify failure
	TestTrue(TEXT("BindWidgetEvent should fail for non-existent component"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention failed to find widget component"),
		Result.GetError().Contains(TEXT("Failed to find widget component")));

	// Cleanup
	UEditorAssetLibrary::DeleteAsset(AssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceSetTextBlockBindingTest,
	"UnrealMCP.Widget.SetTextBlockBinding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceSetTextBlockBindingTest::RunTest(const FString& Parameters)
{
	// Test: Set text block binding

	// Cleanup any existing widget from previous failed tests
	FString AssetPath = TEXT("/Game/UI/BindingTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	// First create a widget with a text block
	UnrealMCP::FWidgetCreationParams CreateParams;
	CreateParams.Name = TEXT("BindingTestWidget");
	CreateParams.PackagePath = TEXT("/Game/UI");

	UnrealMCP::TResult<UWidgetBlueprint*> CreateResult = UnrealMCP::FWidgetService::CreateWidget(CreateParams);
	TestTrue(TEXT("CreateWidget should succeed"), CreateResult.IsSuccess());
	if (!CreateResult.IsSuccess()) return false;

	// Add a text block to bind to
	UnrealMCP::FTextBlockParams TextParams;
	TextParams.WidgetName = TEXT("BindingTestWidget");
	TextParams.TextBlockName = TEXT("BindingTestTextBlock");
	TextParams.Text = TEXT("Initial Text");

	UnrealMCP::TResult<UTextBlock*> TextResult = UnrealMCP::FWidgetService::AddTextBlock(TextParams);
	TestTrue(TEXT("AddTextBlock should succeed"), TextResult.IsSuccess());
	if (!TextResult.IsSuccess())
	{
		// Cleanup before returning
		UEditorAssetLibrary::DeleteAsset(AssetPath);
		return false;
	}

	// Set the binding
	UnrealMCP::FTextBlockBindingParams BindingParams;
	BindingParams.WidgetName = TEXT("BindingTestWidget");
	BindingParams.TextBlockName = TEXT("BindingTestTextBlock");
	BindingParams.BindingProperty = TEXT("PlayerName");
	BindingParams.BindingType = TEXT("Text");

	UnrealMCP::FVoidResult BindingResult = UnrealMCP::FWidgetService::SetTextBlockBinding(BindingParams);

	// Verify success
	TestTrue(TEXT("SetTextBlockBinding should succeed"), BindingResult.IsSuccess());

	// Cleanup
	UEditorAssetLibrary::DeleteAsset(AssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceSetTextBlockBindingInvalidWidgetTest,
	"UnrealMCP.Widget.SetTextBlockBindingInvalidWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceSetTextBlockBindingInvalidWidgetTest::RunTest(const FString& Parameters)
{
	// Test: Setting binding on non-existent widget should fail

	// Suppress expected errors from asset loading
	AddExpectedError(TEXT("LoadAsset failed"), EAutomationExpectedErrorFlags::Contains, 1);

	UnrealMCP::FTextBlockBindingParams BindingParams;
	BindingParams.WidgetName = TEXT("NonExistentWidget_XYZ123");
	BindingParams.TextBlockName = TEXT("SomeTextBlock");
	BindingParams.BindingProperty = TEXT("PlayerName");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FWidgetService::SetTextBlockBinding(BindingParams);

	// Verify failure
	TestTrue(TEXT("SetTextBlockBinding should fail for non-existent widget"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceSetTextBlockBindingInvalidTextBlockTest,
	"UnrealMCP.Widget.SetTextBlockBindingInvalidTextBlock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceSetTextBlockBindingInvalidTextBlockTest::RunTest(const FString& Parameters)
{
	// Test: Setting binding on non-existent text block should fail

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/InvalidTextBlockTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	// First create a widget
	UnrealMCP::FWidgetCreationParams CreateParams;
	CreateParams.Name = TEXT("InvalidTextBlockTestWidget");
	CreateParams.PackagePath = TEXT("/Game/UI");

	const UnrealMCP::TResult<UWidgetBlueprint*> CreateResult = UnrealMCP::FWidgetService::CreateWidget(CreateParams);
	TestTrue(TEXT("CreateWidget should succeed"), CreateResult.IsSuccess());
	if (!CreateResult.IsSuccess()) return false;

	// Try to set binding on a non-existent text block
	UnrealMCP::FTextBlockBindingParams BindingParams;
	BindingParams.WidgetName = TEXT("InvalidTextBlockTestWidget");
	BindingParams.TextBlockName = TEXT("NonExistentTextBlock_XYZ123");
	BindingParams.BindingProperty = TEXT("PlayerName");

	const UnrealMCP::FVoidResult Result = UnrealMCP::FWidgetService::SetTextBlockBinding(BindingParams);

	// Verify failure
	TestTrue(TEXT("SetTextBlockBinding should fail for non-existent text block"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention failed to find TextBlock"),
		Result.GetError().Contains(TEXT("Failed to find TextBlock")));

	// Cleanup
	UEditorAssetLibrary::DeleteAsset(AssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceGetWidgetClassTest,
	"UnrealMCP.Widget.GetWidgetClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceGetWidgetClassTest::RunTest(const FString& Parameters)
{
	// Test: Get widget class from existing widget

	// Cleanup any existing widget from previous failed tests
	const FString AssetPath = TEXT("/Game/UI/GetClassTestWidget");
	if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
	{
		UEditorAssetLibrary::DeleteAsset(AssetPath);
	}

	// First create a widget
	UnrealMCP::FWidgetCreationParams CreateParams;
	CreateParams.Name = TEXT("GetClassTestWidget");
	CreateParams.PackagePath = TEXT("/Game/UI");

	const UnrealMCP::TResult<UWidgetBlueprint*> CreateResult = UnrealMCP::FWidgetService::CreateWidget(CreateParams);
	TestTrue(TEXT("CreateWidget should succeed"), CreateResult.IsSuccess());
	if (!CreateResult.IsSuccess()) return false;

	// Get the widget class
	UnrealMCP::FAddWidgetToViewportParams GetClassParams;
	GetClassParams.WidgetName = TEXT("GetClassTestWidget");
	GetClassParams.ZOrder = 0;

	UnrealMCP::TResult<UClass*> ClassResult = UnrealMCP::FWidgetService::GetWidgetClass(GetClassParams);

	// Verify success
	TestTrue(TEXT("GetWidgetClass should succeed"), ClassResult.IsSuccess());
	const UClass* WidgetClass = ClassResult.GetValue();
	TestTrue(TEXT("WidgetClass should not be null"), WidgetClass != nullptr);
	if (WidgetClass)
	{
		TestTrue(TEXT("WidgetClass should be a child of UserWidget"), WidgetClass->IsChildOf(UUserWidget::StaticClass()));
	}

	// Cleanup
	UEditorAssetLibrary::DeleteAsset(AssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWidgetServiceGetInvalidWidgetClassTest,
	"UnrealMCP.Widget.GetInvalidWidgetClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FWidgetServiceGetInvalidWidgetClassTest::RunTest(const FString& Parameters)
{
	// Test: Getting widget class from non-existent widget should fail

	// Suppress expected errors from asset loading
	AddExpectedError(TEXT("LoadAsset failed"), EAutomationExpectedErrorFlags::Contains, 1);

	UnrealMCP::FAddWidgetToViewportParams GetClassParams;
	GetClassParams.WidgetName = TEXT("NonExistentWidget_XYZ123");
	GetClassParams.ZOrder = 0;

	const UnrealMCP::TResult<UClass*> Result = UnrealMCP::FWidgetService::GetWidgetClass(GetClassParams);

	// Verify failure
	TestTrue(TEXT("GetWidgetClass should fail for non-existent widget"), Result.IsFailure());
	TestTrue(TEXT("Error message should mention not found"),
		Result.GetError().Contains(TEXT("not found")));

	return true;
}