#include "Commands/Blueprint/GetComponentProperties.h"
#include "Services/BlueprintIntrospectionService.h"
#include "Core/CommonUtils.h"
#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace UnrealMCP {

	auto FGetComponentPropertiesCommand::Execute(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject> {
		// Parse parameters
		if (!Params->HasField(TEXT("blueprint_name")) || !Params->HasField(TEXT("component_name"))) {
			return FCommonUtils::CreateErrorResponse(TEXT("Missing required parameters: blueprint_name and component_name"));
		}

		const FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
		const FString ComponentName = Params->GetStringField(TEXT("component_name"));

		// Find the blueprint
		const UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *FBlueprintIntrospectionService::GetBlueprintPath(BlueprintName));
		if (!Blueprint) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found"), *BlueprintName));
		}

		// Find the component in the blueprint
		const USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
		if (!SCS) {
			return FCommonUtils::CreateErrorResponse(TEXT("Blueprint has no construction script"));
		}

		const USCS_Node* TargetNode = nullptr;
		for (const USCS_Node* Node : SCS->GetAllNodes()) {
			if (Node && Node->GetVariableName().ToString() == ComponentName) {
				TargetNode = Node;
				break;
			}
		}

		if (!TargetNode || !TargetNode->ComponentTemplate) {
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Component '%s' not found in blueprint"), *ComponentName));
		}

		// Build properties object
		const auto PropertiesObj = MakeShared<FJsonObject>();
		UActorComponent* ComponentTemplate = TargetNode->ComponentTemplate;

		// Basic info
		PropertiesObj->SetStringField(TEXT("name"), TargetNode->GetVariableName().ToString());
		PropertiesObj->SetStringField(TEXT("type"), ComponentTemplate->GetClass()->GetName());
		PropertiesObj->SetStringField(TEXT("class_path"), ComponentTemplate->GetClass()->GetPathName());

		// Transform properties (if SceneComponent)
		if (const USceneComponent* SceneComp = Cast<USceneComponent>(ComponentTemplate)) {
			const auto TransformObj = MakeShared<FJsonObject>();

			FVector Location = SceneComp->GetRelativeLocation();
			auto LocationArray = TArray<TSharedPtr<FJsonValue>>();
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.X));
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Y));
			LocationArray.Add(MakeShared<FJsonValueNumber>(Location.Z));
			TransformObj->SetArrayField(TEXT("location"), LocationArray);

			FRotator Rotation = SceneComp->GetRelativeRotation();
			auto RotationArray = TArray<TSharedPtr<FJsonValue>>();
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Pitch));
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Yaw));
			RotationArray.Add(MakeShared<FJsonValueNumber>(Rotation.Roll));
			TransformObj->SetArrayField(TEXT("rotation"), RotationArray);

			FVector Scale = SceneComp->GetRelativeScale3D();
			auto ScaleArray = TArray<TSharedPtr<FJsonValue>>();
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.X));
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Y));
			ScaleArray.Add(MakeShared<FJsonValueNumber>(Scale.Z));
			TransformObj->SetArrayField(TEXT("scale"), ScaleArray);

			PropertiesObj->SetObjectField(TEXT("transform"), TransformObj);
			PropertiesObj->SetBoolField(TEXT("mobility"), SceneComp->Mobility == EComponentMobility::Movable);
		}

		// Mesh properties (if StaticMeshComponent)
		if (const UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(ComponentTemplate)) {
			if (MeshComp->GetStaticMesh()) {
				PropertiesObj->SetStringField(TEXT("static_mesh"), MeshComp->GetStaticMesh()->GetPathName());
			}
			PropertiesObj->SetBoolField(TEXT("cast_shadow"), MeshComp->CastShadow);
		}

		// Skeletal mesh properties
		if (const USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(ComponentTemplate)) {
			if (SkelComp->GetSkeletalMeshAsset()) {
				PropertiesObj->SetStringField(TEXT("skeletal_mesh"), SkelComp->GetSkeletalMeshAsset()->GetPathName());
			}
		}

		// Physics properties (if PrimitiveComponent)
		if (const UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(ComponentTemplate)) {
			const auto PhysicsObj = MakeShared<FJsonObject>();
			PhysicsObj->SetBoolField(TEXT("simulate_physics"), PrimComp->IsSimulatingPhysics());
			PhysicsObj->SetBoolField(TEXT("enable_gravity"), PrimComp->IsGravityEnabled());
			PhysicsObj->SetNumberField(TEXT("mass"), PrimComp->GetMass());
			PhysicsObj->SetNumberField(TEXT("linear_damping"), PrimComp->GetLinearDamping());
			PhysicsObj->SetNumberField(TEXT("angular_damping"), PrimComp->GetAngularDamping());
			PhysicsObj->SetStringField(TEXT("collision_profile"), PrimComp->GetCollisionProfileName().ToString());
			PropertiesObj->SetObjectField(TEXT("physics"), PhysicsObj);
		}

		// Light properties (if LightComponent)
		if (const ULightComponent* LightComp = Cast<ULightComponent>(ComponentTemplate)) {
			const auto LightObj = MakeShared<FJsonObject>();
			LightObj->SetNumberField(TEXT("intensity"), LightComp->Intensity);

			FLinearColor Color = LightComp->GetLightColor();
			auto ColorArray = TArray<TSharedPtr<FJsonValue>>();
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.R));
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.G));
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.B));
			ColorArray.Add(MakeShared<FJsonValueNumber>(Color.A));
			LightObj->SetArrayField(TEXT("color"), ColorArray);

			LightObj->SetBoolField(TEXT("cast_shadows"), LightComp->CastShadows);
			PropertiesObj->SetObjectField(TEXT("light"), LightObj);
		}

		// Movement properties (if CharacterMovementComponent)
		if (const UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(ComponentTemplate)) {
			const auto MovementObj = MakeShared<FJsonObject>();
			MovementObj->SetNumberField(TEXT("max_walk_speed"), MovementComp->MaxWalkSpeed);
			MovementObj->SetNumberField(TEXT("max_acceleration"), MovementComp->MaxAcceleration);
			MovementObj->SetNumberField(TEXT("jump_z_velocity"), MovementComp->JumpZVelocity);
			MovementObj->SetNumberField(TEXT("gravity_scale"), MovementComp->GravityScale);
			PropertiesObj->SetObjectField(TEXT("movement"), MovementObj);
		}

		return FCommonUtils::CreateSuccessResponse([&](const TSharedPtr<FJsonObject>& Data) {
			Data->SetObjectField(TEXT("properties"), PropertiesObj);
		});
	}

} // namespace UnrealMCP
