#include "GameAnimationSample/Vehicles/BuggyPawn.h"

#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MoverExamplesCharacter.h"
#include "UObject/ConstructorHelpers.h"

namespace BuggyModel
{
	constexpr float Scale = 0.2f;
	const FVector Center(34.0795f, 23.735f, 0.0f);
	const FRotator Rotation(0.0f, -90.0f, 0.0f);
}

ABuggyPawn::ABuggyPawn()
{
	VehicleMass = 350.0f;
	EngineForce = 600000.0f;
	BrakeForce = 400000.0f;
	TireGrip = 2500.0f;
	TireFrictionLimit = 2.0f;
	RollingResistance = 200.0f;
	SuspensionRestLength = 35.0f;
	SuspensionTravel = 25.0f;
	SpringStiffness = 15000.0f;
	SpringDamping = 2500.0f;
	MaxSuspensionForce = 200000.0f;
	WheelRadius = 24.0f;
	WheelWidth = 12.5f;
	WheelBase = 116.0f;
	TrackWidth = 102.0f;
	BodyCollision->InitBoxExtent(FVector(105.0f, 65.0f, 35.0f));
	BumperMaterial->Restitution = 0.25f;
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyAsset(TEXT("/Game/Vehicles/Buggy/Car_body.Car_body"));
	if (BodyAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(BodyAsset.Object);
	}
	BodyMesh->SetRelativeLocation(MapModelPoint(FVector(33.738235f, -27.596130f, 242.515530f)));
	BodyMesh->SetRelativeRotation(BuggyModel::Rotation);
	BodyMesh->SetRelativeScale3D(FVector(BuggyModel::Scale));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SeatAsset(TEXT("/Game/Vehicles/Buggy/kart_body_2.kart_body_2"));
	if (SeatAsset.Succeeded())
	{
		SeatMesh->SetStaticMesh(SeatAsset.Object);
	}
	SeatMesh->SetRelativeLocation(MapModelPoint(FVector(34.079472f, 97.540482f, 193.376324f)));
	SeatMesh->SetRelativeRotation(BuggyModel::Rotation);
	SeatMesh->SetRelativeScale3D(FVector(BuggyModel::Scale));
	SeatMesh->SetVisibility(true);

	struct FPartDefinition
	{
		FName ComponentName;
		const TCHAR* AssetName;
		FVector Center;
	};
	const FPartDefinition PartDefinitions[] = {
		{TEXT("FrontBumper"), TEXT("front_low"), FVector(37.211117f, 337.281235f, 121.505394f)},
		{TEXT("Grille"), TEXT("grid_low"), FVector(36.377594f, 357.986984f, 90.892101f)},
		{TEXT("LeftHandle"), TEXT("handle_1"), FVector(192.550125f, 314.487839f, 106.811350f)},
		{TEXT("RightHandle"), TEXT("handle_2"), FVector(-117.019840f, 313.954529f, 108.631657f)},
		{TEXT("LeftRearHandle"), TEXT("handle_3"), FVector(232.595459f, -261.246857f, 127.199223f)},
		{TEXT("RightRearHandle"), TEXT("handle_4"), FVector(-164.038666f, -261.492882f, 128.326355f)},
		{TEXT("RollCage"), TEXT("pipes_1_low"), FVector(34.079552f, -130.285851f, 242.194641f)},
		{TEXT("HeadLamp"), TEXT("lamp_low"), FVector(32.631426f, 292.087097f, 242.249413f)},
		{TEXT("SteeringWheel"), TEXT("controlWheel"), FVector(34.079493f, 115.875759f, 313.527473f)},
		{TEXT("NumberDecal"), TEXT("Decal1"), FVector(32.531921f, 343.917603f, 157.976589f)},
	};

	for (const FPartDefinition& Definition : PartDefinitions)
	{
		UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Definition.ComponentName);
		Part->SetupAttachment(BodyCollision);
		Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Part->SetRelativeLocation(MapModelPoint(Definition.Center));
		Part->SetRelativeRotation(BuggyModel::Rotation);
		Part->SetRelativeScale3D(FVector(BuggyModel::Scale));
		Part->SetVisibility(true);
		const FString AssetPath = FString::Printf(TEXT("/Game/Vehicles/Buggy/%s.%s"), Definition.AssetName, Definition.AssetName);
		ConstructorHelpers::FObjectFinder<UStaticMesh> PartAsset(*AssetPath);
		if (PartAsset.Succeeded())
		{
			Part->SetStaticMesh(PartAsset.Object);
		}
		ModelParts.Add(Part);
	}

	const TCHAR* WheelAssets[NumWheels] = {TEXT("Tire_2"), TEXT("Tire_4"), TEXT("Tire_1"), TEXT("Tire_3")};
	WheelSourceCenters[0] = FVector(277.008369f, 311.951935f, 104.989418f);
	WheelSourceCenters[1] = FVector(-207.017090f, 315.996796f, 104.990668f);
	WheelSourceCenters[2] = FVector(292.777878f, -264.480659f, 104.989724f);
	WheelSourceCenters[3] = FVector(-232.486755f, -267.097595f, 104.989939f);
	for (int32 Index = 0; Index < NumWheels; ++Index)
	{
		const FString AssetPath = FString::Printf(TEXT("/Game/Vehicles/Buggy/%s.%s"), WheelAssets[Index], WheelAssets[Index]);
		ConstructorHelpers::FObjectFinder<UStaticMesh> WheelAsset(*AssetPath);
		if (WheelAsset.Succeeded())
		{
			WheelMeshes[Index]->SetStaticMesh(WheelAsset.Object);
			WheelMeshes[Index]->SetVisibility(true);
		}
	}

	SeatPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SeatPoint"));
	SeatPoint->SetupAttachment(BodyCollision);
	SeatPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
	ExitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ExitPoint"));
	ExitPoint->SetupAttachment(BodyCollision);
	ExitPoint->SetRelativeLocation(FVector(0.0f, -120.0f, -10.0f));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SittingAnimation(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Interactions/Bench/M_interaction_bench_idle_loop_Montage.M_interaction_bench_idle_loop_Montage"));
	if (SittingAnimation.Succeeded())
	{
		DriverAnimation = SittingAnimation.Object;
	}
}

FVector ABuggyPawn::MapModelPoint(const FVector& Point)
{
	const FVector Relative = Point - BuggyModel::Center;
	return FVector(
		Relative.Y * BuggyModel::Scale,
		-Relative.X * BuggyModel::Scale,
		Point.Z * BuggyModel::Scale - 21.0f);
}

void ABuggyPawn::UpdateWheelVisual(int32 WheelIndex)
{
	if (!WheelMeshes[WheelIndex])
	{
		return;
	}

	const FBumperCarWheelState& Wheel = WheelStates[WheelIndex];
	const FVector WheelCenter = MapModelPoint(WheelSourceCenters[WheelIndex]);
	const FVector TargetCenter = GetWheelAnchorLocal(WheelIndex) - FVector(0.0f, 0.0f, Wheel.SuspensionLength);
	WheelMeshes[WheelIndex]->SetRelativeLocation(TargetCenter - WheelCenter);
	WheelMeshes[WheelIndex]->SetRelativeScale3D(FVector(BuggyModel::Scale));

	const FQuat SteeringRotation(FVector::UpVector, FMath::DegreesToRadians(Wheel.SteeringAngle));
	const FQuat RollingRotation(FVector::RightVector, FMath::DegreesToRadians(Wheel.SpinAngle));
	const FQuat ModelRotation(BuggyModel::Rotation);
	WheelMeshes[WheelIndex]->SetRelativeRotation((SteeringRotation * RollingRotation * ModelRotation).Rotator());
}

void ABuggyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("BumperCar_Interact"), IE_Pressed, this, &ABuggyPawn::ExitVehicleInput);
}

bool ABuggyPawn::EnterVehicle(APawn* Character)
{
	if (bHasDriver || !Character || !Character->GetController())
	{
		return false;
	}

	DriverPawn = Character;
	DriverController = Character->GetController();
	bDriverTickEnabled = Character->IsActorTickEnabled();
	bDriverCollisionEnabled = Character->GetActorEnableCollision();
	Character->SetActorTickEnabled(false);
	Character->SetActorEnableCollision(false);
	Character->AttachToComponent(SeatPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Character->SetActorRelativeLocation(FVector::ZeroVector);
	Character->SetActorRelativeRotation(FRotator::ZeroRotator);
	Character->SetActorHiddenInGame(bHideDriver);

	if (AMoverExamplesCharacter* MoverCharacter = Cast<AMoverExamplesCharacter>(Character))
	{
		MoverCharacter->RequestMoveByVelocity(FVector::ZeroVector);
	}
	if (ACharacter* CMCCharacter = Cast<ACharacter>(Character))
	{
		CMCCharacter->GetCharacterMovement()->StopMovementImmediately();
		CMCCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
		if (DriverAnimation && CMCCharacter->GetMesh() && CMCCharacter->GetMesh()->GetAnimInstance())
		{
			CMCCharacter->GetMesh()->GetAnimInstance()->Montage_Play(DriverAnimation);
		}
	}

	TargetThrottle = 0.0f;
	TargetSteering = 0.0f;
	bHasDriver = true;
	DriverController->Possess(this);
	return true;
}

bool ABuggyPawn::ExitVehicle()
{
	if (!bHasDriver || !DriverPawn)
	{
		return false;
	}

	APawn* Character = DriverPawn;
	AController* OriginalController = DriverController;
	FVector ExitLocation = ExitPoint->GetComponentLocation();
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BuggyExit), false, this);
	QueryParams.AddIgnoredActor(Character);
	const FCollisionShape Capsule = FCollisionShape::MakeCapsule(40.0f, 90.0f);
	if (GetWorld()->OverlapAnyTestByChannel(ExitLocation, FQuat::Identity, ECC_Pawn, Capsule, QueryParams))
	{
		ExitLocation = GetActorLocation() + GetActorRightVector() * 160.0f + FVector(0.0f, 0.0f, 30.0f);
	}

	Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Character->SetActorLocation(ExitLocation, false, nullptr, ETeleportType::TeleportPhysics);
	Character->SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));
	Character->SetActorTickEnabled(bDriverTickEnabled);
	Character->SetActorEnableCollision(bDriverCollisionEnabled);
	Character->SetActorHiddenInGame(false);

	if (AMoverExamplesCharacter* MoverCharacter = Cast<AMoverExamplesCharacter>(Character))
	{
		MoverCharacter->RequestMoveByVelocity(FVector::ZeroVector);
	}
	if (ACharacter* CMCCharacter = Cast<ACharacter>(Character))
	{
		CMCCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		if (DriverAnimation && CMCCharacter->GetMesh() && CMCCharacter->GetMesh()->GetAnimInstance())
		{
			CMCCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, DriverAnimation);
		}
	}

	bHasDriver = false;
	DriverPawn = nullptr;
	DriverController = nullptr;
	if (OriginalController)
	{
		OriginalController->Possess(Character);
	}
	return true;
}
