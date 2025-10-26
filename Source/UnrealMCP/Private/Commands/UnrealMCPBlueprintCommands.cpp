#include "Commands/UnrealMCPBlueprintCommands.h"

#include "Commands/Blueprint/CreateBlueprint.h"
#include "Commands/Blueprint/CompileBlueprint.h"
#include "Commands/Blueprint/SpawnActorBlueprint.h"
#include "Commands/Blueprint/SetComponentProperty.h"
#include "Commands/Blueprint/SetPhysicsProperties.h"
#include "Commands/Blueprint/SetBlueprintProperty.h"
#include "Commands/Blueprint/SetStaticMeshProperties.h"
#include "Commands/Blueprint/SetPawnProperties.h"
#include "Commands/Blueprint/AddComponent.h"
#include "Commands/Blueprint/ListBlueprints.h"
#include "Commands/Blueprint/BlueprintIntrospectionCommands.h"
#include "Commands/Blueprint/GetComponentProperties.h"
#include "Commands/Blueprint/GetBlueprintFunctions.h"
#include "Commands/Blueprint/RemoveComponent.h"
#include "Commands/Blueprint/RenameComponent.h"
#include "Commands/Blueprint/SetComponentTransform.h"
#include "Commands/Blueprint/GetComponentHierarchy.h"
#include "Commands/Blueprint/DeleteBlueprint.h"
#include "Commands/Blueprint/DuplicateBlueprint.h"
#include "Commands/Blueprint/RemoveVariable.h"
#include "Commands/Blueprint/SetVariableDefaultValue.h"
#include "Commands/Blueprint/SetVariableMetadata.h"
#include "Commands/Blueprint/RenameVariable.h"
#include "Commands/Blueprint/AddFunction.h"
#include "Commands/Blueprint/RemoveFunction.h"
#include "Commands/Blueprint/AddFunctionParameter.h"
#include "Commands/Blueprint/SetFunctionReturnType.h"
#include "Commands/Blueprint/SetFunctionMetadata.h"
#include "Core/CommonUtils.h"

FUnrealMCPBlueprintCommands::FUnrealMCPBlueprintCommands() {
	using namespace UnrealMCP;

	CommandHandlers.Add(TEXT("create_blueprint"), &FCreateBlueprint::Handle);
	CommandHandlers.Add(TEXT("compile_blueprint"), &FCompileBlueprint::Handle);
	CommandHandlers.Add(TEXT("spawn_blueprint_actor"), &FSpawnActorBlueprint::Handle);
	CommandHandlers.Add(TEXT("add_component_to_blueprint"), &FAddComponent::Handle);
	CommandHandlers.Add(TEXT("set_component_property"), &FSetComponentProperty::Handle);
	CommandHandlers.Add(TEXT("set_physics_properties"), &FSetPhysicsProperties::Handle);
	CommandHandlers.Add(TEXT("set_blueprint_property"), &FSetBlueprintProperty::Handle);
	CommandHandlers.Add(TEXT("set_static_mesh_properties"), &FSetStaticMeshProperties::Handle);
	CommandHandlers.Add(TEXT("set_pawn_properties"), &FSetPawnProperties::Handle);

	CommandHandlers.Add(TEXT("list_blueprints"), &FListBlueprintsCommand::Execute);
	CommandHandlers.Add(TEXT("blueprint_exists"), &FBlueprintExistsCommand::Execute);
	CommandHandlers.Add(TEXT("get_blueprint_info"), &FGetBlueprintInfoCommand::Execute);
	CommandHandlers.Add(TEXT("get_blueprint_components"), &FGetBlueprintComponentsCommand::Execute);
	CommandHandlers.Add(TEXT("get_blueprint_variables"), &FGetBlueprintVariablesCommand::Execute);
	CommandHandlers.Add(TEXT("get_blueprint_path"), &FGetBlueprintPathCommand::Execute);
	CommandHandlers.Add(TEXT("get_component_properties"), &FGetComponentPropertiesCommand::Execute);
	CommandHandlers.Add(TEXT("get_blueprint_functions"), &FGetBlueprintFunctionsCommand::Execute);
	CommandHandlers.Add(TEXT("get_component_hierarchy"), &FGetComponentHierarchyCommand::Execute);

	// Component management commands
	CommandHandlers.Add(TEXT("remove_component"), &FRemoveComponentCommand::Execute);
	CommandHandlers.Add(TEXT("rename_component"), &FRenameComponentCommand::Execute);
	CommandHandlers.Add(TEXT("set_component_transform"), &FSetComponentTransformCommand::Execute);

	// Blueprint asset management commands
	CommandHandlers.Add(TEXT("delete_blueprint"), &FDeleteBlueprintCommand::Execute);
	CommandHandlers.Add(TEXT("duplicate_blueprint"), &FDuplicateBlueprintCommand::Execute);

	// Variable management commands
	CommandHandlers.Add(TEXT("remove_variable"), &FRemoveVariableCommand::Execute);
	CommandHandlers.Add(TEXT("set_variable_default_value"), &FSetVariableDefaultValueCommand::Execute);
	CommandHandlers.Add(TEXT("set_variable_metadata"), &FSetVariableMetadataCommand::Execute);
	CommandHandlers.Add(TEXT("rename_variable"), &FRenameVariableCommand::Execute);

	// Function management commands
	CommandHandlers.Add(TEXT("add_function"), &FAddFunctionCommand::Execute);
	CommandHandlers.Add(TEXT("remove_function"), &FRemoveFunctionCommand::Execute);
	CommandHandlers.Add(TEXT("add_function_parameter"), &FAddFunctionParameterCommand::Execute);
	CommandHandlers.Add(TEXT("set_function_return_type"), &FSetFunctionReturnTypeCommand::Execute);
	CommandHandlers.Add(TEXT("set_function_metadata"), &FSetFunctionMetadataCommand::Execute);
}

TSharedPtr<FJsonObject> FUnrealMCPBlueprintCommands::HandleCommand(
	const FString& CommandType,
	const TSharedPtr<FJsonObject>& Params
) {
	if (const auto* Handler = CommandHandlers.Find(CommandType)) {
		return (*Handler)(Params);
	}

	return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown blueprint command: %s"), *CommandType));
}
