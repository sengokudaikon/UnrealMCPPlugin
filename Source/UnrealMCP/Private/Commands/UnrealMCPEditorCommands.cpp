#include "Commands/UnrealMCPEditorCommands.h"
#include "Commands/CommonUtils.h"
#include "Commands/Editor/GetActorsInLevel.h"
#include "Commands/Editor/FindActorsByName.h"
#include "Commands/Editor/SpawnActor.h"
#include "Commands/Editor/DeleteActor.h"
#include "Commands/Editor/SetActorTransform.h"
#include "Commands/Editor/GetActorProperties.h"
#include "Commands/Editor/SetActorProperty.h"
#include "Commands/Blueprint/SpawnActorBlueprint.h"
#include "Commands/Editor/FocusViewport.h"
#include "Commands/Editor/TakeScreenshot.h"

FUnrealMCPEditorCommands::FUnrealMCPEditorCommands() {
	CommandHandlers.Add(TEXT("get_actors_in_level"), &FGetActorsInLevel::Handle);
	CommandHandlers.Add(TEXT("find_actors_by_name"), &FFindActorsByName::Handle);
	CommandHandlers.Add(TEXT("spawn_actor"), &FSpawnActor::Handle);
	CommandHandlers.Add(TEXT("delete_actor"), &FDeleteActor::Handle);
	CommandHandlers.Add(TEXT("set_actor_transform"), &FSetActorTransform::Handle);
	CommandHandlers.Add(TEXT("get_actor_properties"), &FGetActorProperties::Handle);
	CommandHandlers.Add(TEXT("set_actor_property"), &FSetActorProperty::Handle);
	CommandHandlers.Add(TEXT("spawn_blueprint_actor"), &FSpawnActorBlueprint::Handle);
	CommandHandlers.Add(TEXT("focus_viewport"), &FFocusViewport::Handle);
	CommandHandlers.Add(TEXT("take_screenshot"), &FTakeScreenshot::Handle);
}

auto FUnrealMCPEditorCommands::HandleCommand(
	const FString& CommandType,
	const TSharedPtr<FJsonObject>& Params
) -> TSharedPtr<FJsonObject> {
	if (CommandType == TEXT("create_actor")) {
		UE_LOG(LogTemp,
		       Warning,
		       TEXT(
			       "'create_actor' command is deprecated and will be removed in a future version. Please use 'spawn_actor' instead."
		       ));
		return FSpawnActor::Handle(Params);
	}

	if (const auto* Handler = CommandHandlers.Find(CommandType)) {
		return (*Handler)(Params);
	}

	return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown editor command: %s"), *CommandType));
}
