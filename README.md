# UnrealMCP C++ Plugin

Unreal Engine 5.5 plugin providing C++ APIs for the Model Context Protocol (MCP) bridge, enabling programmatic control of Unreal Editor operations through JSON commands.

## Core Architecture

### UUnrealMCPBridge
The main subsystem that manages MCP communication and command routing.

**Public Methods:**
- `StartServer()` - Start the MCP TCP server
- `StopServer()` - Stop the MCP server
- `IsRunning()` - Check if server is active
- `ExecuteCommand(CommandType, Params)` - Execute a command with JSON parameters

## Command APIs

### Editor Commands
Level and actor management operations.

**Available Commands:**
- `get_actors_in_level` - List all actors in current level
- `find_actors_by_name` - Find actors by name pattern
- `spawn_actor` - Create new actors with transform
- `delete_actor` - Remove actors from level
- `set_actor_transform` - Modify actor position, rotation, scale
- `get_actor_properties` - Retrieve actor property values
- `set_actor_property` - Set actor property values
- `focus_viewport` - Focus camera on specific targets
- `take_screenshot` - Capture viewport screenshots

### Blueprint Commands
Blueprint creation, modification, and compilation.

**Available Commands:**
- `create_blueprint` - Create new Blueprint classes
- `compile_blueprint` - Compile Blueprint assets
- `spawn_blueprint_actor` - Spawn Blueprint actors in level
- `add_component_to_blueprint` - Add components to Blueprints
- `set_component_property` - Set component property values
- `set_blueprint_property` - Set Blueprint property values
- `set_static_mesh_properties` - Configure static mesh components
- `set_physics_properties` - Configure physics simulation
- `set_pawn_properties` - Configure pawn-specific properties

### Blueprint Node Commands
Visual scripting node operations.

**Available Commands:**
- `connect_blueprint_nodes` - Connect Blueprint graph nodes
- `add_blueprint_self_reference` - Add self reference nodes
- `add_blueprint_get_self_component_reference` - Add component reference nodes
- `find_blueprint_nodes` - Search for specific nodes
- `add_blueprint_event_node` - Add event nodes
- `add_blueprint_input_action_node` - Add input action nodes
- `add_blueprint_function_node` - Add function call nodes
- `add_blueprint_variable` - Create variable nodes

### Input Commands
Enhanced input system management.

**Available Commands:**
- `create_input_mapping` - Create legacy input mappings
- `create_enhanced_input_action` - Create enhanced input actions
- `create_input_mapping_context` - Create input mapping contexts
- `add_enhanced_input_mapping` - Add mappings to contexts
- `remove_enhanced_input_mapping` - Remove specific mappings
- `apply_mapping_context` - Apply contexts at runtime
- `remove_mapping_context` - Remove contexts at runtime
- `clear_all_mapping_contexts` - Clear all active contexts

### UMG Widget Commands
User interface widget creation and management.

**Available Commands:**
- `create_umg_widget_blueprint` - Create widget blueprints
- `add_text_block_to_widget` - Add text elements
- `add_button_to_widget` - Add interactive buttons
- `bind_widget_event` - Bind widget events to functions
- `add_widget_to_viewport` - Display widgets in game
- `set_text_block_binding` - Bind text to data properties

## Data Types

### Core Parameter Structures
All commands use strongly-typed parameter structures defined in `Core/MCPTypes.h`:

- `FBlueprintCreationParams` - Blueprint creation parameters
- `FBlueprintSpawnParams` - Blueprint actor spawning
- `FComponentParams` - Component addition parameters
- `FPropertyParams` - Property modification parameters
- `FPhysicsParams` - Physics configuration
- `FStaticMeshParams` - Static mesh setup
- `FWidgetCreationParams` - Widget creation
- `FTextBlockParams` - Text block configuration
- `FButtonParams` - Button configuration
- `FWidgetEventBindingParams` - Event binding
- `FInputActionParams` - Input action creation
- `FInputMappingContextParams` - Mapping context setup

### Result Types
All operations return `TResult<T>` types for error handling:
- Success operations return wrapped values
- Failed operations return descriptive error messages

## Integration

### Command Registration
Commands are registered using a registry pattern in each command handler class:

```cpp
// Example from FUnrealMCPEditorCommands
CommandHandlers.Add(TEXT("spawn_actor"), &FSpawnActor::Handle);
```

### Parameter Parsing
All command handlers parse JSON parameters using the `FromJson` static methods on parameter structures:

```cpp
auto Result = FSpawnParams::FromJson(Params);
if (!Result.IsSuccess()) {
    return CreateErrorResult(Result.GetError());
}
```

### Error Handling
Consistent error handling across all commands using the `Result<T>` pattern:
- Validation errors return immediately
- Runtime errors are logged and returned
- Success responses contain operation results

## Services

### ActorService
High-level actor management operations with validation and error handling.

### BlueprintService
Blueprint asset operations including creation, compilation, and modification.

### BlueprintGraphService
Visual scripting node manipulation and graph management.

### InputService
Enhanced input system integration and management.

### ViewportService
Viewport control and screenshot capabilities.

### WidgetService
UMG widget creation and runtime management.

## Usage Pattern

Commands are executed through the bridge subsystem:

```cpp
// Get bridge instance
UUnrealMCPBridge* Bridge = GEditor->GetEditorSubsystem<UUnrealMCPBridge>();

// Create JSON parameters
TSharedPtr<FJsonObject> Params = MakeShared<FJsonObject>();
Params->SetStringField(TEXT("actor_name"), TEXT("MyActor"));

// Execute command
TSharedPtr<FJsonObject> Result = Bridge->ExecuteCommand(TEXT("spawn_actor"), Params);
```