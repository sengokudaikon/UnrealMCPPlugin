#include "Commands/UnrealMCPWidgetCommands.h"
#include "Commands/UMG/CreateUMGWidgetBlueprint.h"
#include "Commands/UMG/AddTextBlockToWidget.h"
#include "Commands/UMG/AddWidgetToViewport.h"
#include "Commands/UMG/AddButtonToWidget.h"
#include "Commands/UMG/BindWidgetEvent.h"
#include "Commands/UMG/SetTextBlockBinding.h"
#include "Commands/CommonUtils.h"

FUnrealMCPWidgetCommands::FUnrealMCPWidgetCommands()
{
	CommandHandlers.Add(TEXT("create_umg_widget_blueprint"), &FCreateUMGWidgetBlueprint::Handle);
	CommandHandlers.Add(TEXT("add_text_block_to_widget"), &FAddTextBlockToWidget::Handle);
	CommandHandlers.Add(TEXT("add_widget_to_viewport"), &FAddWidgetToViewport::Handle);
	CommandHandlers.Add(TEXT("add_button_to_widget"), &FAddButtonToWidget::Handle);
	CommandHandlers.Add(TEXT("bind_widget_event"), &FBindWidgetEvent::Handle);
	CommandHandlers.Add(TEXT("set_text_block_binding"), &FSetTextBlockBinding::Handle);
}

TSharedPtr<FJsonObject> FUnrealMCPWidgetCommands::HandleCommand(
	const FString& CommandType,
	const TSharedPtr<FJsonObject>& Params
)
{
	if (const auto* Handler = CommandHandlers.Find(CommandType))
	{
		return (*Handler)(Params);
	}

	return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown UMG command: %s"), *CommandType));
}

