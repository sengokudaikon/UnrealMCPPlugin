#include "Commands/UnrealMCPBlueprintCommands.h"

#include "Commands/Blueprint/AddComponent.h"
#include "Commands/Blueprint/AddFunction.h"
#include "Commands/Blueprint/AddFunctionParameter.h"
#include "Commands/Blueprint/BlueprintExists.h"
#include "Commands/Blueprint/CompileBlueprint.h"
#include "Commands/Blueprint/CreateBlueprint.h"
#include "Commands/Blueprint/DeleteBlueprint.h"
#include "Commands/Blueprint/DuplicateBlueprint.h"
#include "Commands/Blueprint/GetBlueprintComponents.h"
#include "Commands/Blueprint/GetBlueprintFunctions.h"
#include "Commands/Blueprint/GetBlueprintInfo.h"
#include "Commands/Blueprint/GetBlueprintPath.h"
#include "Commands/Blueprint/GetBlueprintVariables.h"
#include "Commands/Blueprint/GetComponentHierarchy.h"
#include "Commands/Blueprint/GetComponentProperties.h"
#include "Commands/Blueprint/ListBlueprints.h"
#include "Commands/Blueprint/RemoveComponent.h"
#include "Commands/Blueprint/RemoveFunction.h"
#include "Commands/Blueprint/RemoveVariable.h"
#include "Commands/Blueprint/RenameComponent.h"
#include "Commands/Blueprint/RenameVariable.h"
#include "Commands/Blueprint/SetBlueprintProperty.h"
#include "Commands/Blueprint/SetComponentProperty.h"
#include "Commands/Blueprint/SetComponentTransform.h"
#include "Commands/Blueprint/SetFunctionMetadata.h"
#include "Commands/Blueprint/SetFunctionReturnType.h"
#include "Commands/Blueprint/SetPawnProperties.h"
#include "Commands/Blueprint/SetPhysicsProperties.h"
#include "Commands/Blueprint/SetStaticMeshProperties.h"
#include "Commands/Blueprint/SetVariableDefaultValue.h"
#include "Commands/Blueprint/SetVariableMetadata.h"
#include "Commands/Blueprint/SpawnActorBlueprint.h"
#include "Core/CommonUtils.h"

namespace UnrealMCP {
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

		// Introspection commands
		CommandHandlers.Add(TEXT("list_blueprints"), &FListBlueprintsCommand::Handle);
		CommandHandlers.Add(TEXT("blueprint_exists"), &FBlueprintExistsCommand::Handle);
		CommandHandlers.Add(TEXT("get_blueprint_info"), &FGetBlueprintInfoCommand::Handle);
		CommandHandlers.Add(TEXT("get_blueprint_components"), &FGetBlueprintComponentsCommand::Handle);
		CommandHandlers.Add(TEXT("get_blueprint_variables"), &FGetBlueprintVariablesCommand::Handle);
		CommandHandlers.Add(TEXT("get_blueprint_path"), &FGetBlueprintPathCommand::Handle);
		CommandHandlers.Add(TEXT("get_component_properties"), &FGetComponentPropertiesCommand::Handle);
		CommandHandlers.Add(TEXT("get_blueprint_functions"), &FGetBlueprintFunctionsCommand::Handle);
		CommandHandlers.Add(TEXT("get_component_hierarchy"), &FGetComponentHierarchyCommand::Handle);

		// Component management commands
		CommandHandlers.Add(TEXT("remove_component"), &FRemoveComponentCommand::Handle);
		CommandHandlers.Add(TEXT("rename_component"), &FRenameComponentCommand::Handle);
		CommandHandlers.Add(TEXT("set_component_transform"), &FSetComponentTransformCommand::Handle);

		// Blueprint asset management commands
		CommandHandlers.Add(TEXT("delete_blueprint"), &FDeleteBlueprintCommand::Handle);
		CommandHandlers.Add(TEXT("duplicate_blueprint"), &FDuplicateBlueprintCommand::Handle);

		// Variable management commands
		CommandHandlers.Add(TEXT("remove_variable"), &FRemoveVariableCommand::Handle);
		CommandHandlers.Add(TEXT("set_variable_default_value"), &FSetVariableDefaultValueCommand::Handle);
		CommandHandlers.Add(TEXT("set_variable_metadata"), &FSetVariableMetadataCommand::Handle);
		CommandHandlers.Add(TEXT("rename_variable"), &FRenameVariableCommand::Handle);

		// Function management commands
		CommandHandlers.Add(TEXT("add_function"), &FAddFunctionCommand::Handle);
		CommandHandlers.Add(TEXT("remove_function"), &FRemoveFunctionCommand::Handle);
		CommandHandlers.Add(TEXT("add_function_parameter"), &FAddFunctionParameterCommand::Handle);
		CommandHandlers.Add(TEXT("set_function_return_type"), &FSetFunctionReturnTypeCommand::Handle);
		CommandHandlers.Add(TEXT("set_function_metadata"), &FSetFunctionMetadataCommand::Handle);
	}

	auto FUnrealMCPBlueprintCommands::HandleCommand(
		const FString& CommandType,
		const TSharedPtr<FJsonObject>& Params
	) -> TSharedPtr<FJsonObject> {
		if (const auto* Handler = CommandHandlers.Find(CommandType)) {
			return (*Handler)(Params);
		}

		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown blueprint command: %s"), *CommandType));
	}
}
