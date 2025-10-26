# UnrealMCP Feature Tracking

## Implemented Features

### Blueprint Introspection and Querying
- [x] `list_blueprints(path, recursive)` - List all blueprints in a directory
- [x] `blueprint_exists(blueprint_name)` - Check if a blueprint exists
- [x] `get_blueprint_info(blueprint_name)` - Get comprehensive blueprint metadata
- [x] `get_blueprint_path(blueprint_name)` - Get full asset path for a blueprint
- [x] `get_blueprint_components(blueprint_name)` - List all components in a blueprint
- [x] `get_blueprint_variables(blueprint_name)` - List all variables with types and visibility
- [x] `get_blueprint_functions(blueprint_name)` - List all custom functions with parameters
- [x] `get_component_properties(blueprint_name, component_name)` - Get detailed component properties
- [x] `get_component_hierarchy(blueprint_name)` - Get component parent-child tree structure

### Component Management
- [x] `add_component_to_blueprint(blueprint_name, component_type, component_name, location, rotation, scale, properties)` - Add components
- [x] `remove_component(blueprint_name, component_name)` - Delete components
- [x] `rename_component(blueprint_name, old_name, new_name)` - Rename components
- [x] `set_component_transform(blueprint_name, component_name, location, rotation, scale)` - Update transform
- [x] `set_static_mesh_properties(blueprint_name, component_name, static_mesh)` - Configure static mesh
- [x] `set_physics_properties(blueprint_name, component_name, simulate_physics, gravity_enabled, mass, linear_damping, angular_damping)` - Configure physics

### Variable Management
- [x] `remove_variable(blueprint_name, variable_name)` - Delete variables
- [x] `rename_variable(blueprint_name, old_name, new_name)` - Rename variables with reference updates
- [x] `set_variable_default_value(blueprint_name, variable_name, value)` - Set default values (supports bool, int, float, string, Vector, Rotator)
- [x] `set_variable_metadata(blueprint_name, variable_name, tooltip, category, expose_on_spawn, instance_editable, blueprint_read_only)` - Configure variable metadata

### Function Management
- [x] `add_function(blueprint_name, function_name)` - Create custom functions
- [x] `remove_function(blueprint_name, function_name)` - Delete functions
- [x] `add_function_parameter(blueprint_name, function_name, param_name, param_type, is_reference)` - Add parameters to functions
- [x] `set_function_return_type(blueprint_name, function_name, return_type)` - Configure return value type
- [x] `set_function_metadata(blueprint_name, function_name, category, tooltip, pure)` - Configure function metadata (category, tooltip, pure flag)

### Blueprint Asset Management
- [x] `create_blueprint(name, parent_class)` - Create new blueprints
- [x] `delete_blueprint(blueprint_name)` - Delete blueprint assets
- [x] `duplicate_blueprint(source_name, new_name, path)` - Clone blueprints
- [x] `compile_blueprint(blueprint_name)` - Compile blueprints
- [x] `spawn_blueprint_actor(blueprint_name, actor_name, location, rotation, scale)` - Spawn actors from blueprints

### Registry and Extensibility
- [x] `get_supported_parent_classes()` - Query all available parent classes dynamically (100+ classes via reflection)
- [x] `get_supported_component_types()` - Query all available component types dynamically (50+ types via reflection)
- [x] `get_available_api_methods()` - Get list of all API methods organized by category

### Blueprint Graph (Limited)
- [x] `add_input_action_event(blueprint_name, action_name, position)` - Add input action event nodes
- [x] `add_function_call_node(blueprint_name, function_name, target, position)` - Add function call nodes
- [x] `connect_nodes(blueprint_name, source_node, source_pin, target_node, target_pin)` - Connect node pins

### Widget/UMG
- [x] `create_umg_widget_blueprint(widget_name, parent_class, path)` - Create UMG widgets
- [x] `add_text_block_to_widget(widget_name, text_block_name, text, position, size, font_size, color)` - Add text blocks
- [x] `add_button_to_widget(widget_name, button_name, text, position, size, font_size, color, background_color)` - Add buttons
- [x] `bind_widget_event(widget_name, widget_component_name, event_name, function_name)` - Bind widget events
- [x] `add_widget_to_viewport(widget_name, z_order)` - Display widgets at runtime
- [x] `set_text_block_binding(widget_name, text_block_name, binding_property, binding_type)` - Set up property bindings

### Enhanced Input System
- [x] `create_enhanced_input_action(name, value_type, path)` - Create input action assets
- [x] `create_input_mapping_context(name, path)` - Create mapping context assets
- [x] `add_enhanced_input_mapping(context_path, action_path, key)` - Add key mappings
- [x] `apply_mapping_context(context_path, priority)` - Apply contexts at runtime
- [x] `remove_mapping_context(context_path)` - Remove contexts at runtime
- [x] `clear_all_mapping_contexts()` - Clear all contexts

### Actor and Level Operations
- [x] `get_actors_in_level()` - Get all actors in current level
- [x] `find_actors_by_name(pattern)` - Find actors by name pattern
- [x] `get_actor_properties(name)` - Get actor properties
- [x] `spawn_actor(name, actor_type, location, rotation, scale)` - Spawn actors
- [x] `delete_actor(name)` - Delete actors from level
- [x] `set_actor_transform(name, location, rotation, scale)` - Set actor transform

### Editor and Viewport
- [x] `focus_viewport(target, location, distance, orientation)` - Focus viewport on target or location
- [x] `take_screenshot(filename, resolution, show_ui)` - Capture viewport screenshots

## Planned Features

### Blueprint Graph Extensions (HIGH PRIORITY)

#### Control Flow Nodes
- [ ] `add_branch_node(blueprint_name, position)` - Add conditional logic
- [ ] `add_sequence_node(blueprint_name, position, num_outputs)` - Add execution sequencing
- [ ] `add_for_each_loop_node(blueprint_name, position)` - Add ForEach loop
- [ ] `add_while_loop_node(blueprint_name, position)` - Add While loop
- [ ] `add_delay_node(blueprint_name, duration, position)` - Add delay node
- [ ] `add_do_once_node(blueprint_name, position)` - Add DoOnce gate

#### Math and Logic Nodes
- [ ] `add_math_operation_node(blueprint_name, operation_type, position)` - Add math operations (Add, Subtract, Multiply, Divide, etc.)
- [ ] `add_comparison_node(blueprint_name, comparison_type, position)` - Add comparison operations (Equal, NotEqual, Greater, Less, etc.)
- [ ] `add_logic_gate_node(blueprint_name, gate_type, position)` - Add logic gates (AND, OR, NOT, XOR)
- [ ] `add_select_node(blueprint_name, position)` - Add Select node (ternary operator)

#### Advanced Graph Operations
- [ ] `add_custom_event(blueprint_name, event_name, parameters)` - Create custom events
- [ ] `add_timeline_node(blueprint_name, timeline_name, position)` - Add timeline node
- [ ] `add_cast_node(blueprint_name, target_class, position)` - Add type cast node
- [ ] `add_macro_node(blueprint_name, macro_name, position)` - Add macro instance
- [ ] `add_reroute_node(blueprint_name, position)` - Add reroute node for organization
- [ ] `add_literal_node(blueprint_name, value_type, value, position)` - Add constant value nodes
- [ ] `remove_node(blueprint_name, node_guid)` - Delete nodes from graph
- [ ] `disconnect_pins(blueprint_name, source_node_id, source_pin, target_node_id, target_pin)` - Break connections
- [ ] `set_node_position(blueprint_name, node_guid, position)` - Move nodes in graph
- [ ] `get_node_info(blueprint_name, node_guid)` - Get node details (type, pins, parameters)
- [ ] `get_pin_info(blueprint_name, node_guid, pin_name)` - Get pin metadata
- [ ] `get_blueprint_graph_nodes(blueprint_name)` - Get all nodes in event graph with connections

#### Struct and Data Handling
- [ ] `add_make_struct_node(blueprint_name, struct_type, position)` - Create struct construction nodes
- [ ] `add_break_struct_node(blueprint_name, struct_type, position)` - Create struct deconstruction nodes
- [ ] `add_make_array_node(blueprint_name, position)` - Create array construction nodes
- [ ] `add_array_operation_node(blueprint_name, operation_type, position)` - Array operations (Add, Remove, Find, etc.)

### Variable Extensions (MEDIUM PRIORITY)
- [ ] `add_variable_get_node(blueprint_name, variable_name, position)` - Add getter node
- [ ] `add_variable_set_node(blueprint_name, variable_name, position)` - Add setter node
- [ ] `get_variable_references(blueprint_name, variable_name)` - Find all nodes using variable
- [ ] `add_array_variable(blueprint_name, variable_name, element_type)` - Create array variables
- [ ] `add_map_variable(blueprint_name, variable_name, key_type, value_type)` - Create map variables
- [ ] `add_struct_variable(blueprint_name, variable_name, struct_type)` - Create struct variables

### Function Graph Operations (MEDIUM PRIORITY)
- [ ] `get_function_graph(blueprint_name, function_name)` - Get nodes in function graph
- [ ] `add_return_node(blueprint_name, function_name, position)` - Add return node to function

### Component Extensions (MEDIUM PRIORITY)
- [ ] `duplicate_component(blueprint_name, source_component, new_name)` - Clone components
- [ ] `set_component_parent(blueprint_name, component_name, parent_component_name)` - Reparent components
- [ ] `set_skeletal_mesh_properties(blueprint_name, component_name, skeletal_mesh, anim_blueprint)` - Configure skeletal mesh
- [ ] `set_material_properties(blueprint_name, component_name, materials)` - Set materials on components
- [ ] `attach_component_to_socket(blueprint_name, component_name, target_component, socket_name)` - Socket-based attachment

### Blueprint Asset Operations (MEDIUM PRIORITY)
- [ ] `rename_blueprint(old_name, new_name)` - Rename blueprints
- [ ] `reparent_blueprint(blueprint_name, new_parent_class)` - Change parent class
- [ ] `save_blueprint(blueprint_name)` - Explicitly save blueprint to disk
- [ ] `get_blueprint_compile_errors(blueprint_name)` - Get compilation errors and warnings

### Material and Texture Management (MEDIUM PRIORITY)
- [ ] `create_material_instance(base_material, instance_name)` - Create material instances
- [ ] `set_material_parameter(material_name, parameter_name, value)` - Set material parameters
- [ ] `assign_material_to_component(blueprint_name, component_name, material_slot, material_path)` - Assign materials
- [ ] `import_texture(file_path, asset_path, compression_settings)` - Import textures
- [ ] `get_material_parameters(material_name)` - List available parameters

### Animation System (MEDIUM PRIORITY)
- [ ] `set_anim_blueprint(blueprint_name, skeletal_component_name, anim_blueprint)` - Assign animation blueprints
- [ ] `set_skeletal_mesh(blueprint_name, component_name, skeletal_mesh)` - Assign skeletal meshes
- [ ] `play_animation(blueprint_name, skeletal_component_name, animation_asset)` - Play animations
- [ ] `create_anim_blueprint(name, skeleton)` - Create animation blueprints

### Enhanced Input Extensions (LOW PRIORITY)
- [ ] `list_input_actions(path)` - List all input actions
- [ ] `list_input_contexts(path)` - List all mapping contexts
- [ ] `get_input_action_info(action_path)` - Get action metadata
- [ ] `get_mapping_context_info(context_path)` - Get context mappings with triggers/modifiers
- [ ] `add_input_modifier(context_path, action_path, modifier_type, params)` - Add input modifiers (negate, swizzle, etc.)
- [ ] `add_input_trigger(context_path, action_path, trigger_type, params)` - Add input triggers (hold, tap, chord, etc.)
- [ ] `set_input_mapping_metadata(context_path, action_path, key, metadata)` - Configure mapping settings

### Widget/UMG Extensions (MEDIUM PRIORITY)
- [ ] `add_image_to_widget(widget_name, image_name, texture, position, size)` - Add image widgets
- [ ] `add_progress_bar_to_widget(widget_name, progress_bar_name, position, size)` - Add progress bars
- [ ] `add_slider_to_widget(widget_name, slider_name, min_value, max_value, position, size)` - Add sliders
- [ ] `add_vertical_box_to_widget(widget_name, box_name)` - Add vertical layout
- [ ] `add_horizontal_box_to_widget(widget_name, box_name)` - Add horizontal layout
- [ ] `add_grid_panel_to_widget(widget_name, grid_name)` - Add grid layout
- [ ] `add_scroll_box_to_widget(widget_name, scroll_name)` - Add scroll containers
- [ ] `remove_widget_child(widget_name, child_name)` - Remove widgets
- [ ] `set_widget_style(widget_name, widget_child_name, style_params)` - Configure widget appearance
- [ ] `set_widget_visibility(widget_name, widget_child_name, visibility)` - Show/hide widgets
- [ ] `get_widget_hierarchy(widget_name)` - Get widget tree structure
- [ ] `bind_widget_property(widget_name, widget_child_name, property_name, binding_function)` - Advanced property binding

### Level and World Operations (LOW PRIORITY)
- [ ] `get_current_level_name()` - Get active level name
- [ ] `create_level(level_name)` - Create new levels
- [ ] `load_level(level_name)` - Switch levels
- [ ] `save_level()` - Save current level
- [ ] `get_level_actors_by_type(actor_type)` - Get all actors of specific type
- [ ] `set_world_settings_property(property_name, value)` - Configure world settings
- [ ] `set_game_mode(game_mode_path)` - Set level's game mode

### Asset Import and Export (LOW PRIORITY)
- [ ] `import_static_mesh(fbx_path, asset_path, import_options)` - Import static meshes
- [ ] `import_skeletal_mesh(fbx_path, asset_path, skeleton, import_options)` - Import skeletal meshes
- [ ] `import_sound(audio_path, asset_path)` - Import audio files
- [ ] `export_asset(asset_path, export_path, export_format)` - Export assets

### Debugging and Analysis (LOW PRIORITY)
- [ ] `get_blueprint_dependencies(blueprint_name)` - Get referenced assets
- [ ] `find_blueprint_references(blueprint_name)` - Find what references this blueprint
- [ ] `validate_blueprint_graph(blueprint_name)` - Check for orphaned nodes and missing connections
- [ ] `get_blueprint_performance_metrics(blueprint_name)` - Complexity analysis
- [ ] `set_breakpoint(blueprint_name, node_guid)` - Add debug breakpoints
- [ ] `remove_breakpoint(blueprint_name, node_guid)` - Remove debug breakpoints

### Batch Operations (MEDIUM PRIORITY)
- [ ] `batch_create_components(blueprint_name, components_list)` - Create multiple components atomically
- [ ] `batch_connect_nodes(blueprint_name, connections_list)` - Connect multiple nodes efficiently
- [ ] `batch_set_properties(blueprint_name, properties_list)` - Set multiple properties atomically
- [ ] `create_blueprint_from_template(template_name, new_name, replacements)` - Template-based blueprint creation

### Collision and Physics Extensions (LOW PRIORITY)
- [ ] `set_collision_profile(blueprint_name, component_name, profile_name)` - Set collision presets
- [ ] `set_collision_responses(blueprint_name, component_name, response_map)` - Custom collision responses
- [ ] `add_physics_constraint(blueprint_name, constraint_name, component1, component2)` - Add physics constraints
- [ ] `set_physics_constraint_properties(blueprint_name, constraint_name, properties)` - Configure constraints

### Property System Improvements (HIGH PRIORITY)
- [ ] `set_typed_property(blueprint_name, property_name, property_type, value)` - Type-safe property setting
- [ ] `get_property_value(blueprint_name, property_name)` - Read property values
- [ ] `get_available_properties(blueprint_name)` - List all available properties with types

### Interface and Inheritance (MEDIUM PRIORITY)
- [ ] `add_blueprint_interface(blueprint_name, interface_name)` - Implement interfaces
- [ ] `remove_blueprint_interface(blueprint_name, interface_name)` - Remove interfaces
- [ ] `get_blueprint_interfaces(blueprint_name)` - List implemented interfaces
- [ ] `override_parent_function(blueprint_name, function_name)` - Override parent class functions

### Streaming and LOD (LOW PRIORITY)
- [ ] `set_static_mesh_lod_settings(blueprint_name, component_name, lod_settings)` - Configure LODs
- [ ] `set_component_streaming_distance(blueprint_name, component_name, distance)` - Streaming settings
- [ ] `set_hlod_settings(blueprint_name, component_name, hlod_layer)` - HLOD configuration

## Implementation Notes

### Code Style and Patterns
- All commands use `FCommonUtils::CreateSuccessResponse()` and `FCommonUtils::CreateErrorResponse()` for consistent response formatting
- Include order: Local headers, then `Core/CommonUtils.h`, then Engine headers
- Blueprint operations use `FBlueprintEditorUtils` and `FKismetEditorUtilities` for safe modifications
- Always call `FBlueprintEditorUtils::MarkBlueprintAsModified()` and `FKismetEditorUtilities::CompileBlueprint()` after modifications
- Use `FBlueprintIntrospectionService::GetBlueprintPath()` for blueprint path resolution
- Command routing uses TMap for O(1) lookup performance

### Architecture
- C++ plugin provides core command handlers via UnrealMCP plugin
- Python service layer in `Python/services/` provides high-level API
- MCP tools in `Python/tools/` expose functionality to Claude Code
- Communication via JSON-based command/response protocol
- Dynamic class discovery via MCPRegistry using Unreal's reflection system

### Current Limitations
- Graph manipulation is limited (no control flow nodes, cannot remove nodes)
- Some property operations are string-based and not fully type-safe
- No batch operations available yet
- Limited debugging and validation tools
