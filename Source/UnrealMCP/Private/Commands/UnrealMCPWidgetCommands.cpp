#include "Commands/UnrealMCPWidgetCommands.h"
#include "Commands/Widget/AddButtonToWidget.h"
#include "Commands/Widget/AddTextBlockToWidget.h"
#include "Commands/Widget/AddWidgetToViewport.h"
#include "Commands/Widget/BindWidgetEvent.h"
#include "Commands/Widget/CreateUMGWidgetBlueprint.h"
#include "Commands/Widget/SetTextBlockBinding.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {

	FUnrealMCPWidgetCommands::FUnrealMCPWidgetCommands() {
		CommandHandlers.Add(TEXT("create_umg_widget_blueprint"), &FCreateUMGWidgetBlueprint::Handle);
		CommandHandlers.Add(TEXT("add_text_block_to_widget"), &FAddTextBlockToWidget::Handle);
		CommandHandlers.Add(TEXT("add_widget_to_viewport"), &FAddWidgetToViewport::Handle);
		CommandHandlers.Add(TEXT("add_button_to_widget"), &FAddButtonToWidget::Handle);
		CommandHandlers.Add(TEXT("bind_widget_event"), &FBindWidgetEvent::Handle);
		CommandHandlers.Add(TEXT("set_text_block_binding"), &FSetTextBlockBinding::Handle);
	}

	auto FUnrealMCPWidgetCommands::HandleCommand(
		const FString& CommandType,
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		if (const auto* Handler = CommandHandlers.Find(CommandType)) {
			return (*Handler)(Params);
		}

		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown UMG command: %s"), *CommandType));
	}
}
