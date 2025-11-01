#include "Services/WidgetService.h"
#include "Core/ErrorTypes.h"
#include "EdGraphSchema_K2.h"
#include "Editor.h"
#include "EditorAssetLibrary.h"
#include "K2Node_Event.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_VariableGet.h"
#include "WidgetBlueprint.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

namespace UnrealMCP {
	auto FWidgetService::CreateWidget(const FWidgetCreationParams& Params) -> TResult<UWidgetBlueprint*> {
		// Validate input parameters
		if (Params.Name.IsEmpty()) {
			return TResult<UWidgetBlueprint*>::Failure(EErrorCode::InvalidInput, TEXT("CreateWidget"));
		}
		if (Params.PackagePath.IsEmpty()) {
			return TResult<UWidgetBlueprint*>::Failure(EErrorCode::InvalidInput, TEXT("CreateWidget"));
		}

		const FString FullPath = Params.PackagePath / Params.Name;

		// Check if asset already exists
		if (UEditorAssetLibrary::DoesAssetExist(FullPath)) {
			return TResult<UWidgetBlueprint*>::Failure(EErrorCode::FailedToCreateWidget, Params.Name);
		}

		// Create package
		UPackage* Package = CreatePackage(*FullPath);
		if (!Package) {
			return TResult<UWidgetBlueprint*>::Failure(EErrorCode::FailedToCreateWidget, Params.Name);
		}

		// Create Widget Blueprint
		UBlueprint* NewBlueprint = FKismetEditorUtilities::CreateBlueprint(
			UUserWidget::StaticClass(),
			Package,
			FName(*Params.Name),
			BPTYPE_Normal,
			UWidgetBlueprint::StaticClass(),
			UBlueprintGeneratedClass::StaticClass(),
			FName("CreateWidget")
		);

		UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(NewBlueprint);
		if (!WidgetBlueprint) {
			return TResult<UWidgetBlueprint*>::Failure(EErrorCode::FailedToCreateWidget, Params.Name);
		}

		// Add a default Canvas Panel if one doesn't exist
		if (!WidgetBlueprint->WidgetTree->RootWidget) {
			UCanvasPanel* RootCanvas = WidgetBlueprint->WidgetTree->ConstructWidget<UCanvasPanel>(
				UCanvasPanel::StaticClass());
			WidgetBlueprint->WidgetTree->RootWidget = RootCanvas;
		}

		// Mark the package dirty and notify asset registry
		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(WidgetBlueprint);

		// Compile the blueprint
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

		return TResult<UWidgetBlueprint*>::Success(WidgetBlueprint);
	}

	auto FWidgetService::AddTextBlock(const FTextBlockParams& Params) -> TResult<UTextBlock*> {
		// Validate input parameters
		if (Params.WidgetName.IsEmpty()) {
			return TResult<UTextBlock*>::Failure(EErrorCode::InvalidInput, TEXT("AddTextBlock"));
		}
		if (Params.TextBlockName.IsEmpty()) {
			return TResult<UTextBlock*>::Failure(EErrorCode::InvalidInput, TEXT("AddTextBlock"));
		}

		const FString FullPath = ResolveWidgetPath(Params.WidgetName);

		UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(UEditorAssetLibrary::LoadAsset(FullPath));
		if (!WidgetBlueprint) {
			return TResult<UTextBlock*>::Failure(EErrorCode::WidgetNotFound, Params.WidgetName);
		}

		// Validate canvas root before proceeding
		const FVoidResult ValidationResult = ValidateCanvasRoot(WidgetBlueprint);
		if (ValidationResult.IsFailure()) {
			return TResult<UTextBlock*>::Failure(ValidationResult.GetError());
		}

		// Create Text Block widget
		UTextBlock* TextBlock = WidgetBlueprint->WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			FName(*Params.TextBlockName)
		);
		if (!TextBlock) {
			return TResult<UTextBlock*>::Failure(EErrorCode::FailedToCreateWidget, Params.TextBlockName);
		}

		// Register the widget with the blueprint's GUID system
		const FGuid NewGuid = FGuid::NewGuid();
		WidgetBlueprint->WidgetVariableNameToGuidMap.Add(TextBlock->GetFName(), NewGuid);

		// Set initial text
		TextBlock->SetText(FText::FromString(Params.Text));

		// Set font size
		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = Params.FontSize;
		TextBlock->SetFont(FontInfo);

		// Set color if specified
		if (Params.Color.IsSet()) {
			TextBlock->SetColorAndOpacity(FSlateColor(Params.Color.GetValue()));
		}

		// Add to canvas panel and apply transform
		UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetBlueprint->WidgetTree->RootWidget);
		UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(TextBlock);
		ApplyCanvasSlotTransform(PanelSlot, Params.Position, Params.Size);

		// Mark the package dirty and compile
		WidgetBlueprint->MarkPackageDirty();
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

		return TResult<UTextBlock*>::Success(TextBlock);
	}

	auto FWidgetService::AddButton(const FButtonParams& Params) -> TResult<UButton*> {
		// Validate input parameters
		if (Params.WidgetName.IsEmpty()) {
			return TResult<UButton*>::Failure(EErrorCode::InvalidInput, TEXT("AddButton"));
		}
		if (Params.ButtonName.IsEmpty()) {
			return TResult<UButton*>::Failure(EErrorCode::InvalidInput, TEXT("AddButton"));
		}

		const FString FullPath = ResolveWidgetPath(Params.WidgetName);

		UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(UEditorAssetLibrary::LoadAsset(FullPath));
		if (!WidgetBlueprint) {
			return TResult<UButton*>::Failure(EErrorCode::WidgetNotFound, Params.WidgetName);
		}

		// Validate canvas root before proceeding
		const FVoidResult ValidationResult = ValidateCanvasRoot(WidgetBlueprint);
		if (ValidationResult.IsFailure()) {
			return TResult<UButton*>::Failure(ValidationResult.GetError());
		}

		// Create Button widget
		UButton* Button = WidgetBlueprint->WidgetTree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			FName(*Params.ButtonName)
		);
		if (!Button) {
			return TResult<UButton*>::Failure(EErrorCode::FailedToCreateWidget, Params.ButtonName);
		}

		// Register the widget with the blueprint's GUID system
		const FGuid NewGuid = FGuid::NewGuid();
		WidgetBlueprint->WidgetVariableNameToGuidMap.Add(Button->GetFName(), NewGuid);

		// Add to canvas panel and apply transform
		UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetBlueprint->WidgetTree->RootWidget);
		UCanvasPanelSlot* ButtonSlot = RootCanvas->AddChildToCanvas(Button);
		ApplyCanvasSlotTransform(ButtonSlot, Params.Position, Params.Size);

		// Mark the package dirty and compile
		WidgetBlueprint->MarkPackageDirty();
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

		return TResult<UButton*>::Success(Button);
	}

	auto FWidgetService::BindWidgetEvent(const FWidgetEventBindingParams& Params) -> FVoidResult {
		// Validate input parameters
		if (Params.WidgetName.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("BindWidgetEvent"));
		}
		if (Params.WidgetComponentName.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("BindWidgetEvent"));
		}
		if (Params.EventName.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("BindWidgetEvent"));
		}

		const FString FullPath = ResolveWidgetPath(Params.WidgetName);

		UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(UEditorAssetLibrary::LoadAsset(FullPath));
		if (!WidgetBlueprint) {
			return FVoidResult::Failure(EErrorCode::WidgetNotFound, Params.WidgetName);
		}

		// Get the event graph
		const UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(WidgetBlueprint);
		if (!EventGraph) {
			return FVoidResult::Failure(EErrorCode::FailedToBindWidgetEvent, Params.WidgetName);
		}

		// Find the widget component in the blueprint
		const UWidget* Widget = WidgetBlueprint->WidgetTree->FindWidget(FName(*Params.WidgetComponentName));
		if (!Widget) {
			return FVoidResult::Failure(EErrorCode::ComponentNotFound, Params.WidgetComponentName);
		}

		// Create the event node using the utility
		FKismetEditorUtilities::CreateNewBoundEventForClass(
			Widget->GetClass(),
			FName(*Params.EventName),
			WidgetBlueprint,
			nullptr
		);

		// Mark the package dirty and compile
		WidgetBlueprint->MarkPackageDirty();
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

		return FVoidResult::Success();
	}

	auto FWidgetService::SetTextBlockBinding(const FTextBlockBindingParams& Params) -> FVoidResult {
		// Validate input parameters
		if (Params.WidgetName.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("SetTextBlockBinding"));
		}
		if (Params.TextBlockName.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("SetTextBlockBinding"));
		}
		if (Params.BindingProperty.IsEmpty()) {
			return FVoidResult::Failure(EErrorCode::InvalidInput, TEXT("SetTextBlockBinding"));
		}

		const FString FullPath = ResolveWidgetPath(Params.WidgetName);

		UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(UEditorAssetLibrary::LoadAsset(FullPath));
		if (!WidgetBlueprint) {
			return FVoidResult::Failure(EErrorCode::WidgetNotFound, Params.WidgetName);
		}

		// Find the TextBlock widget
		const UTextBlock* TextBlock = Cast<UTextBlock>(
			WidgetBlueprint->WidgetTree->FindWidget(FName(*Params.TextBlockName)));
		if (!TextBlock) {
			return FVoidResult::Failure(EErrorCode::ComponentNotFound, Params.TextBlockName);
		}

		// Create a variable for binding if it doesn't exist
		FBlueprintEditorUtils::AddMemberVariable(
			WidgetBlueprint,
			FName(*Params.BindingProperty),
			FEdGraphPinType(UEdGraphSchema_K2::PC_Text,
			                NAME_None,
			                nullptr,
			                EPinContainerType::None,
			                false,
			                FEdGraphTerminalType())
		);

		// Check if binding function already exists
		const FString FunctionName = FString::Printf(TEXT("Get%s"), *Params.BindingProperty);
		UEdGraph* FuncGraph = nullptr;

		// Search through existing graphs to see if one with this name already exists
		for (UEdGraph* Graph : WidgetBlueprint->FunctionGraphs) {
			if (Graph && Graph->GetFName() == FName(*FunctionName)) {
				FuncGraph = Graph;
				break;
			}
		}

		// Only create if it doesn't exist
		if (!FuncGraph) {
			FuncGraph = FBlueprintEditorUtils::CreateNewGraph(
				WidgetBlueprint,
				FName(*FunctionName),
				UEdGraph::StaticClass(),
				UEdGraphSchema_K2::StaticClass()
			);

			if (FuncGraph) {
				// Add the function to the blueprint - this may create entry nodes automatically
				FBlueprintEditorUtils::AddFunctionGraph<UClass>(WidgetBlueprint, FuncGraph, false, nullptr);

				// Check if entry node already exists
				UK2Node_FunctionEntry* EntryNode = nullptr;
				for (UEdGraphNode* Node : FuncGraph->Nodes) {
					EntryNode = Cast<UK2Node_FunctionEntry>(Node);
					if (EntryNode) {
						break;
					}
				}

				// Create entry node only if it doesn't exist
				if (!EntryNode) {
					EntryNode = NewObject<UK2Node_FunctionEntry>(FuncGraph);
					FuncGraph->AddNode(EntryNode, false, false);
					EntryNode->NodePosX = 0;
					EntryNode->NodePosY = 0;
					EntryNode->FunctionReference.SetExternalMember(FName(*FunctionName),
					                                               WidgetBlueprint->GeneratedClass);
					EntryNode->AllocateDefaultPins();
				}

				// Create get variable node
				UK2Node_VariableGet* GetVarNode = NewObject<UK2Node_VariableGet>(FuncGraph);
				GetVarNode->VariableReference.SetSelfMember(FName(*Params.BindingProperty));
				FuncGraph->AddNode(GetVarNode, false, false);
				GetVarNode->NodePosX = 200;
				GetVarNode->NodePosY = 0;
				GetVarNode->AllocateDefaultPins();

				// Connect nodes
				UEdGraphPin* EntryThenPin = EntryNode->FindPin(UEdGraphSchema_K2::PN_Then);
				UEdGraphPin* GetVarOutPin = GetVarNode->FindPin(UEdGraphSchema_K2::PN_ReturnValue);
				if (EntryThenPin && GetVarOutPin) {
					EntryThenPin->MakeLinkTo(GetVarOutPin);
				}
			}
		}

		// Mark the package dirty and compile
		WidgetBlueprint->MarkPackageDirty();
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

		return FVoidResult::Success();
	}

	auto FWidgetService::GetWidgetClass(const FAddWidgetToViewportParams& Params) -> TResult<UClass*> {
		// Validate input parameters
		if (Params.WidgetName.IsEmpty()) {
			return TResult<UClass*>::Failure(EErrorCode::InvalidInput, TEXT("GetWidgetClass"));
		}

		const FString FullPath = ResolveWidgetPath(Params.WidgetName);

		const UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(UEditorAssetLibrary::LoadAsset(FullPath));
		if (!WidgetBlueprint) {
			return TResult<UClass*>::Failure(EErrorCode::WidgetNotFound, Params.WidgetName);
		}

		// Get the widget class
		UClass* WidgetClass = WidgetBlueprint->GeneratedClass;
		if (!WidgetClass) {
			return TResult<UClass*>::Failure(EErrorCode::InvalidWidgetType, Params.WidgetName);
		}

		return TResult<UClass*>::Success(WidgetClass);
	}

	auto FWidgetService::EnsureUniqueAssetName(const FString& BaseName, const FString& PackagePath) -> FString {
		FString UniqueName = BaseName;
		FString FullPath = PackagePath / UniqueName;

		// Check if asset exists, and if so, append a number until we find a unique name
		int32 Suffix = 1;
		while (UEditorAssetLibrary::DoesAssetExist(FullPath)) {
			UniqueName = FString::Printf(TEXT("%s_%d"), *BaseName, Suffix++);
			FullPath = PackagePath / UniqueName;
		}

		return UniqueName;
	}

	auto FWidgetService::ResolveWidgetPath(const FString& WidgetName) -> FString {
		// If the name already contains a path separator, assume it's a full path
		if (WidgetName.Contains(TEXT("/"))) {
			return WidgetName;
		}

		// Otherwise, assume it's in /Game/UI directory
		return TEXT("/Game/UI/") + WidgetName;
	}

	auto FWidgetService::ValidateCanvasRoot(const UWidgetBlueprint* WidgetBlueprint) -> FVoidResult {
		if (!WidgetBlueprint || !WidgetBlueprint->WidgetTree) {
			return FVoidResult::Failure(EErrorCode::InvalidWidgetType, TEXT("ValidateCanvasRoot"));
		}

		const UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetBlueprint->WidgetTree->RootWidget);
		if (!RootCanvas) {
			return FVoidResult::Failure(EErrorCode::InvalidWidgetType, TEXT("CanvasPanel"));
		}

		return FVoidResult::Success();
	}

	auto FWidgetService::ApplyCanvasSlotTransform(
		UCanvasPanelSlot* Slot,
		const TOptional<FVector2D>& Position,
		const TOptional<FVector2D>& Size
	) -> void {
		if (!Slot) {
			return;
		}

		if (Position.IsSet()) {
			Slot->SetPosition(Position.GetValue());
		}

		if (Size.IsSet()) {
			Slot->SetSize(Size.GetValue());
		}
	}
}
