#include "UnrealMCPBridge.h"
#include "MCPServerRunnable.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "HAL/RunnableThread.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Camera/CameraActor.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "JsonObjectConverter.h"
#include "GameFramework/Actor.h"
#include "Engine/Selection.h"
#include "Kismet/GameplayStatics.h"
#include "Async/Async.h"
// Add Blueprint related includes
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Factories/BlueprintFactory.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Event.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
// UE5.5 correct includes
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "UObject/Field.h"
#include "UObject/FieldPath.h"
// Blueprint Graph specific includes
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_CallFunction.h"
#include "K2Node_InputAction.h"
#include "K2Node_Self.h"
#include "GameFramework/InputSettings.h"
#include "EditorSubsystem.h"
#include "Subsystems/EditorActorSubsystem.h"
// Include our new command handler classes
#include "Commands/UnrealMCPEditorCommands.h"
#include "Commands/UnrealMCPBlueprintCommands.h"
#include "Commands/UnrealMCPBlueprintNodeCommands.h"
#include "Commands/CommonUtils.h"
#include "Commands/UnrealMCPInputCommands.h"
#include "Commands/UnrealMCPWidgetCommands.h"
#include "Commands/UnrealMCPRegistryCommands.h"
#include "Core/MCPRegistry.h"

// Default settings
#define MCP_SERVER_HOST "127.0.0.1"
#define MCP_SERVER_PORT 55557

UUnrealMCPBridge::UUnrealMCPBridge()
{
    EditorCommands = MakeShared<FUnrealMCPEditorCommands>();
    BlueprintCommands = MakeShared<FUnrealMCPBlueprintCommands>();
    BlueprintNodeCommands = MakeShared<FUnrealMCPBlueprintNodeCommands>();
    InputCommands = MakeShared<FUnrealMCPInputCommands>();
    UMGCommands = MakeShared<FUnrealMCPWidgetCommands>();
    RegistryCommands = MakeShared<FUnrealMCPRegistryCommands>();
    InitializeCommandRouting();
}

UUnrealMCPBridge::~UUnrealMCPBridge()
{
    EditorCommands.Reset();
    BlueprintCommands.Reset();
    BlueprintNodeCommands.Reset();
    InputCommands.Reset();
    UMGCommands.Reset();
    RegistryCommands.Reset();
}

// Initialize subsystem
void UUnrealMCPBridge::Initialize(FSubsystemCollectionBase& Collection)
{
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Initializing"));

    // Initialize the MCP Registry
    UnrealMCP::FMCPRegistry::Initialize();

    bIsRunning = false;
    ListenerSocket = nullptr;
    ConnectionSocket = nullptr;
    ServerThread = nullptr;
    Port = MCP_SERVER_PORT;
    FIPv4Address::Parse(MCP_SERVER_HOST, ServerAddress);

    // Start the server automatically
    StartServer();
}

// Clean up resources when subsystem is destroyed
void UUnrealMCPBridge::Deinitialize()
{
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Shutting down"));
    StopServer();
}

// Start the MCP server
void UUnrealMCPBridge::StartServer()
{
    if (bIsRunning)
    {
        UE_LOG(LogTemp, Warning, TEXT("UnrealMCPBridge: Server is already running"));
        return;
    }

    // Create socket subsystem
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to get socket subsystem"));
        return;
    }

    // Create listener socket
    const TSharedPtr<FSocket> NewListenerSocket = MakeShareable(SocketSubsystem->CreateSocket(NAME_Stream, TEXT("UnrealMCPListener"), false));
    if (!NewListenerSocket.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to create listener socket"));
        return;
    }

    // Allow address reuse for quick restarts
    NewListenerSocket->SetReuseAddr(true);
    NewListenerSocket->SetNonBlocking(true);

    // Bind to address
    const FIPv4Endpoint Endpoint(ServerAddress, Port);
    if (!NewListenerSocket->Bind(*Endpoint.ToInternetAddr()))
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to bind listener socket to %s:%d"), *ServerAddress.ToString(), Port);
        return;
    }

    // Start listening
    if (!NewListenerSocket->Listen(5))
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to start listening"));
        return;
    }

    ListenerSocket = NewListenerSocket;
    bIsRunning = true;
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Server started on %s:%d"), *ServerAddress.ToString(), Port);

    // Start server thread
    ServerThread = FRunnableThread::Create(
        new FMCPServerRunnable(this, ListenerSocket),
        TEXT("UnrealMCPServerThread"),
        0, TPri_Normal
    );

    if (!ServerThread)
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to create server thread"));
        StopServer();
        return;
    }
}

// Stop the MCP server
void UUnrealMCPBridge::StopServer()
{
    if (!bIsRunning)
    {
        return;
    }

    bIsRunning = false;

    // Clean up thread
    if (ServerThread)
    {
        ServerThread->Kill(true);
        delete ServerThread;
        ServerThread = nullptr;
    }

    // Close sockets
    if (ConnectionSocket.IsValid())
    {
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket.Get());
        ConnectionSocket.Reset();
    }

    if (ListenerSocket.IsValid())
    {
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket.Get());
        ListenerSocket.Reset();
    }

    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Server stopped"));
}

void UUnrealMCPBridge::InitializeCommandRouting()
{
    // Ping command
    CommandRoutingMap.Add(TEXT("ping"), ECommandHandlerType::Ping);

    // Editor commands
    CommandRoutingMap.Add(TEXT("get_actors_in_level"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("find_actors_by_name"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("spawn_actor"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("create_actor"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("delete_actor"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("set_actor_transform"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("get_actor_properties"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("set_actor_property"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("spawn_blueprint_actor"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("focus_viewport"), ECommandHandlerType::Editor);
    CommandRoutingMap.Add(TEXT("take_screenshot"), ECommandHandlerType::Editor);

    // Blueprint commands
    CommandRoutingMap.Add(TEXT("create_blueprint"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("add_component_to_blueprint"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("set_component_property"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("set_physics_properties"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("compile_blueprint"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("set_blueprint_property"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("set_static_mesh_properties"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("set_pawn_properties"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("list_blueprints"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("blueprint_exists"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("get_blueprint_info"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("get_blueprint_components"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("get_blueprint_variables"), ECommandHandlerType::Blueprint);
    CommandRoutingMap.Add(TEXT("get_blueprint_path"), ECommandHandlerType::Blueprint);

    // Blueprint node commands
    CommandRoutingMap.Add(TEXT("connect_blueprint_nodes"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("add_blueprint_get_self_component_reference"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("add_blueprint_self_reference"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("find_blueprint_nodes"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("add_blueprint_event_node"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("add_blueprint_input_action_node"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("add_blueprint_function_node"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("add_blueprint_get_component_node"), ECommandHandlerType::BlueprintNode);
    CommandRoutingMap.Add(TEXT("add_blueprint_variable"), ECommandHandlerType::BlueprintNode);

    // Input commands
    CommandRoutingMap.Add(TEXT("create_input_mapping"), ECommandHandlerType::Input);
    CommandRoutingMap.Add(TEXT("create_enhanced_input_action"), ECommandHandlerType::Input);
    CommandRoutingMap.Add(TEXT("create_input_mapping_context"), ECommandHandlerType::Input);
    CommandRoutingMap.Add(TEXT("add_enhanced_input_mapping"), ECommandHandlerType::Input);
    CommandRoutingMap.Add(TEXT("remove_enhanced_input_mapping"), ECommandHandlerType::Input);
    CommandRoutingMap.Add(TEXT("apply_mapping_context"), ECommandHandlerType::Input);
    CommandRoutingMap.Add(TEXT("remove_mapping_context"), ECommandHandlerType::Input);
    CommandRoutingMap.Add(TEXT("clear_all_mapping_contexts"), ECommandHandlerType::Input);

    // UMG/Widget commands
    CommandRoutingMap.Add(TEXT("create_umg_widget_blueprint"), ECommandHandlerType::Widget);
    CommandRoutingMap.Add(TEXT("add_text_block_to_widget"), ECommandHandlerType::Widget);
    CommandRoutingMap.Add(TEXT("add_button_to_widget"), ECommandHandlerType::Widget);
    CommandRoutingMap.Add(TEXT("bind_widget_event"), ECommandHandlerType::Widget);
    CommandRoutingMap.Add(TEXT("set_text_block_binding"), ECommandHandlerType::Widget);
    CommandRoutingMap.Add(TEXT("add_widget_to_viewport"), ECommandHandlerType::Widget);

    // Registry commands
    CommandRoutingMap.Add(TEXT("get_supported_parent_classes"), ECommandHandlerType::Registry);
    CommandRoutingMap.Add(TEXT("get_supported_component_types"), ECommandHandlerType::Registry);
    CommandRoutingMap.Add(TEXT("get_available_api_methods"), ECommandHandlerType::Registry);
}

// Execute a command received from a client
FString UUnrealMCPBridge::ExecuteCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Executing command: %s"), *CommandType);

    // Create a promise to wait for the result
    TPromise<FString> Promise;
    const TFuture<FString> Future = Promise.GetFuture();

    // Queue execution on Game Thread
    AsyncTask(ENamedThreads::GameThread, [this, CommandType, Params, Promise = MoveTemp(Promise)]() mutable
    {
        const TSharedPtr<FJsonObject> ResponseJson = MakeShareable(new FJsonObject);

        try
        {
            TSharedPtr<FJsonObject> ResultJson;

            // O(1) command lookup
            const ECommandHandlerType* HandlerType = CommandRoutingMap.Find(CommandType);
            if (HandlerType)
            {
                switch (*HandlerType)
                {
                    case ECommandHandlerType::Ping:
                        ResultJson = MakeShareable(new FJsonObject);
                        ResultJson->SetStringField(TEXT("message"), TEXT("pong"));
                        break;
                    case ECommandHandlerType::Editor:
                        ResultJson = EditorCommands->HandleCommand(CommandType, Params);
                        break;
                    case ECommandHandlerType::Blueprint:
                        ResultJson = BlueprintCommands->HandleCommand(CommandType, Params);
                        break;
                    case ECommandHandlerType::BlueprintNode:
                        ResultJson = BlueprintNodeCommands->HandleCommand(CommandType, Params);
                        break;
                    case ECommandHandlerType::Input:
                        ResultJson = InputCommands->HandleCommand(CommandType, Params);
                        break;
                    case ECommandHandlerType::Widget:
                        ResultJson = UMGCommands->HandleCommand(CommandType, Params);
                        break;
                    case ECommandHandlerType::Registry:
                        ResultJson = RegistryCommands->HandleCommand(CommandType, Params);
                        break;
                }
            }
            else
            {
                ResponseJson->SetStringField(TEXT("status"), TEXT("error"));
                ResponseJson->SetStringField(TEXT("error"), FString::Printf(TEXT("Unknown command: %s"), *CommandType));

                FString ResultString;
                const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultString);
                FJsonSerializer::Serialize(ResponseJson.ToSharedRef(), Writer);
                Promise.SetValue(ResultString);
                return;
            }

            // Check if the result contains an error
            bool bSuccess = true;
            FString ErrorMessage;

            if (ResultJson->HasField(TEXT("success")))
            {
                bSuccess = ResultJson->GetBoolField(TEXT("success"));
                if (!bSuccess && ResultJson->HasField(TEXT("error")))
                {
                    ErrorMessage = ResultJson->GetStringField(TEXT("error"));
                }
            }

            if (bSuccess)
            {
                // Set success status and include the result
                ResponseJson->SetStringField(TEXT("status"), TEXT("success"));
                ResponseJson->SetObjectField(TEXT("result"), ResultJson);
            }
            else
            {
                // Set error status and include the error message
                ResponseJson->SetStringField(TEXT("status"), TEXT("error"));
                ResponseJson->SetStringField(TEXT("error"), ErrorMessage);
            }
        }
        catch (const std::exception& e)
        {
            ResponseJson->SetStringField(TEXT("status"), TEXT("error"));
            ResponseJson->SetStringField(TEXT("error"), UTF8_TO_TCHAR(e.what()));
        }

        FString ResultString;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultString);
        FJsonSerializer::Serialize(ResponseJson.ToSharedRef(), Writer);
        Promise.SetValue(ResultString);
    });

    return Future.Get();
}