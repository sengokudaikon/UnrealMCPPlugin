# Changelog

## [0.1.0] - 2024-10-30

### Added
- **Blueprint Introspection and Querying**
  - `list_blueprints(path, recursive)` - List all blueprints in a directory
  - `blueprint_exists(blueprint_name)` - Check if a blueprint exists
  - `get_blueprint_info(blueprint_name)` - Get comprehensive blueprint metadata
  - `get_blueprint_path(blueprint_name)` - Get full asset path for a blueprint
  - `get_blueprint_components(blueprint_name)` - List all components in a blueprint
  - `get_blueprint_variables(blueprint_name)` - List all variables with types and visibility

- **Component Management**
  - `add_component_to_blueprint(blueprint_name, component_type, component_name, location, rotation, scale, properties)` - Add components
  - `remove_component(blueprint_name, component_name)` - Delete components
  - `rename_component(blueprint_name, old_name, new_name)` - Rename components
  - `set_component_transform(blueprint_name, component_name, location, rotation, scale)` - Update transform
  - `set_component_property(blueprint_name, component_name, property_name, value)` - Set component properties
  - `set_physics_properties(blueprint_name, component_name, simulate_physics, gravity_enabled, mass, linear_damping, angular_damping)` - Configure physics
  - `get_component_properties(blueprint_name, component_name)` - Get detailed component properties
  - `get_component_hierarchy(blueprint_name)` - Get component parent-child tree structure

- **Variable Management**
  - `remove_variable(blueprint_name, variable_name)` - Delete variables
  - `rename_variable(blueprint_name, old_name, new_name)` - Rename variables with reference updates
  - `set_variable_default_value(blueprint_name, variable_name, value)` - Set default values (supports bool, int, float, string, Vector, Rotator)
  - `set_variable_metadata(blueprint_name, variable_name, tooltip, category, expose_on_spawn, instance_editable, blueprint_read_only)` - Configure variable metadata

- **Function Management**
  - `add_function(blueprint_name, function_name)` - Create custom functions
  - `remove_function(blueprint_name, function_name)` - Delete functions
  - `add_function_parameter(blueprint_name, function_name, param_name, param_type, is_reference)` - Add parameters to functions
  - `set_function_return_type(blueprint_name, function_name, return_type)` - Configure return value type
  - `set_function_metadata(blueprint_name, function_name, category, tooltip, pure)` - Configure function metadata
  - `get_blueprint_functions(blueprint_name)` - List all custom functions with parameters

- **Blueprint Asset Management**
  - `create_blueprint(name, parent_class)` - Create new blueprints
  - `delete_blueprint(blueprint_name)` - Delete blueprint assets
  - `duplicate_blueprint(source_name, new_name, path)` - Clone blueprints
  - `compile_blueprint(blueprint_name)` - Compile blueprints
  - `spawn_blueprint_actor(blueprint_name, actor_name, location, rotation, scale)` - Spawn actors from blueprints

- **Blueprint Graph Operations**
  - `connect_blueprint_nodes(blueprint_name, source_node, source_pin, target_node, target_pin)` - Connect node pins
  - `add_blueprint_event_node(blueprint_name, event_name, position)` - Add event nodes
  - `add_blueprint_input_action_node(blueprint_name, action_name, position)` - Add input action nodes
  - `add_blueprint_function_node(blueprint_name, function_name, target_component, position)` - Add function call nodes
  - `add_blueprint_get_self_component_reference(blueprint_name, position)` - Add self component reference nodes
  - `add_blueprint_self_reference(blueprint_name, position)` - Add self reference nodes
  - `find_blueprint_nodes(blueprint_name, node_type, node_name)` - Find blueprint nodes by criteria

- **Property Management**
  - `set_blueprint_property(blueprint_name, property_name, value)` - Set blueprint properties
  - `set_static_mesh_properties(blueprint_name, component_name, static_mesh)` - Configure static mesh
  - `set_pawn_properties(blueprint_name, pawn_type, collision_enabled, simulation_type)` - Configure pawn properties

- **Registry and Query**
  - `get_supported_parent_classes()` - Query all available parent classes dynamically
  - `get_supported_component_types()` - Query all available component types dynamically
  - `get_available_api_methods()` - Get list of all API methods organized by category