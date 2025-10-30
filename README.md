# UnrealMCP C++ Plugin

Unreal Engine 5.5 plugin providing C++ APIs for the Model Context Protocol (MCP) bridge, enabling programmatic control of Unreal Editor operations through JSON commands.

## 🚀 Quick Start

For interfacing via MCP for your coding agents, go to the [Python MCP Server Repository](https://github.com/sengokudaikon/unreal-mcp)

## 📋 Features Overview

This MCP plugin provides **35+ commands** across 6 categories:
- Blueprint Introspection & Management
- Component Operations & Physics
- Variable & Function Management
- Actor & Level Operations
- Enhanced Input System
- UMG Widget Creation

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
Comprehensive blueprint introspection, creation, and modification.

**Blueprint Introspection:**
- `list_blueprints` - List all blueprints in directory
- `blueprint_exists` - Check if blueprint exists
- `get_blueprint_info` - Get comprehensive blueprint metadata
- `get_blueprint_path` - Get blueprint asset path
- `get_blueprint_components` - List all blueprint components
- `get_blueprint_variables` - List all variables with types
- `get_blueprint_functions` - List all custom functions
- `get_component_hierarchy` - Get component parent-child tree

**Blueprint Asset Management:**
- `create_blueprint` - Create new Blueprint classes
- `delete_blueprint` - Delete blueprint assets
- `duplicate_blueprint` - Clone blueprints with new name
- `compile_blueprint` - Compile Blueprint assets
- `spawn_blueprint_actor` - Spawn Blueprint actors in level

**Variable Management:**
- `remove_variable` - Delete variables
- `rename_variable` - Rename variables with reference updates
- `set_variable_default_value` - Set default values (bool, int, float, string, Vector, Rotator)
- `set_variable_metadata` - Configure variable metadata (tooltip, category, visibility)

**Function Management:**
- `add_function` - Create custom functions
- `remove_function` - Delete functions
- `add_function_parameter` - Add parameters to functions
- `set_function_return_type` - Configure return value type
- `set_function_metadata` - Configure function metadata (category, tooltip, pure)

**Component Management:**
- `add_component_to_blueprint` - Add components to Blueprints
- `remove_component` - Delete components
- `rename_component` - Rename components
- `set_component_transform` - Update component transform
- `set_component_property` - Set component property values
- `set_physics_properties` - Configure physics simulation
- `set_static_mesh_properties` - Configure static mesh components
- `set_pawn_properties` - Configure pawn-specific properties
- `get_component_properties` - Get detailed component properties

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
- `create_enhanced_input_action` - Create enhanced input actions
- `create_input_mapping_context` - Create input mapping contexts
- `add_enhanced_input_mapping` - Add mappings to contexts
- `apply_mapping_context` - Apply contexts at runtime
- `remove_mapping_context` - Remove contexts at runtime
- `clear_all_mapping_contexts` - Clear all active contexts

### Registry & Query Commands
Dynamic discovery of available classes and types.

**Available Commands:**
- `get_supported_parent_classes` - Query all available parent classes (100+ via reflection)
- `get_supported_component_types` - Query all available component types (50+ via reflection)
- `get_available_api_methods` - Get list of all API methods organized by category

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

### MCP Protocol
This plugin implements the Model Context Protocol (MCP) for JSON-based command/response communication. The Python MCP server provides the interface for coding agents to access these capabilities.

**Python MCP Repository:** [sengokudaikon/unreal-mcp](https://github.com/sengokudaikon/unreal-mcp)

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

## Getting Started

1. **Install this plugin** in your Unreal Engine project
2. **Deploy the Python MCP Server** from [sengokudaikon/unreal-mcp](https://github.com/sengokudaikon/unreal-mcp)
3. **Connect your coding agent** to the MCP server to access all blueprint and editor operations
4. **Use available commands** to manipulate blueprints, actors, and game content programmatically