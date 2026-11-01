#include "GameAnimationSample/Vehicles/BumperCarPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/ConstructorHelpers.h"

ABumperCarPawn::ABumperCarPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
	SetRootComponent(BodyCollision);
	BodyCollision->InitBoxExtent(FVector(130.0f, 95.0f, 35.0f));
	BodyCollision->SetCollisionProfileName(TEXT("Vehicle"));
	BodyCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BodyCollision->SetNotifyRigidBodyCollision(true);
	BodyCollision->SetSimulatePhysics(true);
	BodyCollision->SetEnableGravity(true);
	BodyCollision->SetLinearDamping(0.05f);
	BodyCollision->SetAngularDamping(0.6f);

	BumperMaterial = CreateDefaultSubobject<UPhysicalMaterial>(TEXT("BumperMaterial"));
	BumperMaterial->Friction = 0.4f;
	BumperMaterial->Restitution = 0.65f;
	BumperMaterial->RestitutionCombineMode = EFrictionCombineMode::Max;
	BumperMaterial->bOverrideRestitutionCombineMode = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(BodyCollision);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	BodyMesh->SetRelativeScale3D(FVector(2.5f, 1.8f, 0.5f));

	SeatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SeatMesh"));
	SeatMesh->SetupAttachment(BodyCollision);
	SeatMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SeatMesh->SetRelativeLocation(FVector(-30.0f, 0.0f, 55.0f));
	SeatMesh->SetRelativeScale3D(FVector(0.65f, 0.75f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
		SeatMesh->SetStaticMesh(CubeMesh.Object);
	}

	WheelStates.SetNum(NumWheels);
	for (int32 Index = 0; Index < NumWheels; ++Index)
	{
		WheelStates[Index].SuspensionLength = SuspensionRestLength;

		WheelMeshes[Index] = CreateDefaultSubobject<UStaticMeshComponent>(
			*FString::Printf(TEXT("WheelMesh%d"), Index));
		WheelMeshes[Index]->SetupAttachment(BodyCollision);
		WheelMeshes[Index]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WheelMeshes[Index]->SetRelativeScale3D(FVector(0.7f, 0.7f, 0.22f));
		if (CylinderMesh.Succeeded())
		{
			WheelMeshes[Index]->SetStaticMesh(CylinderMesh.Object);
		}
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(BodyCollision);
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	CameraBoom->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ABumperCarPawn::BeginPlay()
{
	Super::BeginPlay();
	BodyCollision->SetSimulatePhysics(true);
	BodyCollision->SetEnableGravity(true);
	BodyCollision->WakeAllRigidBodies();
}

void ABumperCarPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BodyCollision->SetPhysMaterialOverride(BumperMaterial);
	BodyCollision->SetMassOverrideInKg(NAME_None, VehicleMass, true);
	BodyCollision->SetCenterOfMass(FVector(0.0f, 0.0f, -30.0f));
	for (int32 Index = 0; Index < NumWheels; ++Index)
	{
		WheelStates[Index].SuspensionLength = SuspensionRestLength;
		WheelStates[Index].SteeringAngle = 0.0f;
		UpdateWheelVisual(Index);
	}
}

void ABumperCarPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!BodyCollision->IsSimulatingPhysics())
	{
		return;
	}

	BodyCollision->WakeAllRigidBodies();
	Throttle = FMath::FInterpTo(Throttle, TargetThrottle, DeltaSeconds, ThrottleResponse);
	Steering = FMath::FInterpTo(Steering, TargetSteering, DeltaSeconds, SteeringResponse);

	float NormalLoads[NumWheels] = {};
	int32 GroundedWheelCount = 0;
	for (int32 Index = 0; Index < NumWheels; ++Index)
	{
		NormalLoads[Index] = SimulateSuspension(Index);
		GroundedWheelCount += WheelStates[Index].bIsGrounded ? 1 : 0;
	}

	for (int32 Index = 0; Index < NumWheels; ++Index)
	{
		ApplyTireForces(Index, NormalLoads[Index], GroundedWheelCount, DeltaSeconds);
		WheelStates[Index].SpinAngle = FMath::Fmod(
			WheelStates[Index].SpinAngle
				+ FMath::RadiansToDegrees(WheelStates[Index].SurfaceSpeed / WheelRadius) * DeltaSeconds,
			360.0f);
		UpdateWheelVisual(Index);
	}

}

FVector ABumperCarPawn::GetWheelAnchorLocal(int32 WheelIndex) const
{
	const float Forward = WheelIndex < 2 ? WheelBase * 0.5f : -WheelBase * 0.5f;
	const float Right = WheelIndex % 2 == 0 ? -TrackWidth * 0.5f : TrackWidth * 0.5f;
	return FVector(Forward, Right, 0.0f);
}

float ABumperCarPawn::SimulateSuspension(int32 WheelIndex)
{
	FBumperCarWheelState& Wheel = WheelStates[WheelIndex];
	const FVector Anchor = BodyCollision->GetComponentTransform().TransformPosition(GetWheelAnchorLocal(WheelIndex));
	const FVector Up = BodyCollision->GetUpVector();
	const float TraceLength = SuspensionRestLength + SuspensionTravel + WheelRadius;
	const FVector TraceEnd = Anchor - Up * TraceLength;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BumperCarSuspension), false, this);
	QueryParams.bReturnPhysicalMaterial = true;
	FHitResult Hit;
	Wheel.bIsGrounded = GetWorld()->LineTraceSingleByChannel(Hit, Anchor, TraceEnd, ECC_Visibility, QueryParams);

	if (!Wheel.bIsGrounded)
	{
		Wheel.SuspensionLength = SuspensionRestLength + SuspensionTravel;
		Wheel.Compression = 0.0f;
		Wheel.SurfaceSpeed = 0.0f;
		ContactPoints[WheelIndex] = TraceEnd;
		ContactNormals[WheelIndex] = Up;
		ContactSurfaces[WheelIndex] = EPhysicalSurface::SurfaceType_Default;

		if (bDrawSuspensionDebug)
		{
			DrawDebugLine(GetWorld(), Anchor, TraceEnd, FColor::Red, false, -1.0f, 0, 2.0f);
		}
		return 0.0f;
	}

	Wheel.SuspensionLength = FMath::Clamp(
		Hit.Distance - WheelRadius,
		0.0f,
		SuspensionRestLength + SuspensionTravel);
	Wheel.Compression = FMath::Max(0.0f, SuspensionRestLength - Wheel.SuspensionLength);
	ContactPoints[WheelIndex] = Hit.Location;
	ContactNormals[WheelIndex] = Hit.Normal;
	ContactSurfaces[WheelIndex] = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

	float SuspensionForce = 0.0f;
	if (Wheel.Compression > 0.0f)
	{
		const float SuspensionVelocity = FVector::DotProduct(
			BodyCollision->GetPhysicsLinearVelocityAtPoint(Anchor),
			Up);
		SuspensionForce = FMath::Clamp(
			Wheel.Compression * SpringStiffness - SuspensionVelocity * SpringDamping,
			0.0f,
			MaxSuspensionForce);
		BodyCollision->AddForceAtLocation(Up * SuspensionForce, Anchor);
	}

	if (bDrawSuspensionDebug)
	{
		DrawDebugLine(GetWorld(), Anchor, Hit.Location, FColor::Green, false, -1.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), Hit.Location, 5.0f, 8, FColor::Yellow, false, -1.0f, 0, 1.0f);
	}
	return SuspensionForce;
}

void ABumperCarPawn::ApplyTireForces(
	int32 WheelIndex,
	float NormalLoad,
	int32 GroundedWheelCount,
	float DeltaSeconds)
{
	FBumperCarWheelState& Wheel = WheelStates[WheelIndex];
	Wheel.SurfaceSpeed = 0.0f;

	const float SpeedRatio = FMath::Clamp(
		FMath::Abs(BodyCollision->GetPhysicsLinearVelocity().Size()) / MaxForwardSpeed,
		0.0f,
		1.0f);
	Wheel.SteeringAngle = WheelIndex < 2
		? Steering * MaxSteeringAngle * FMath::Lerp(1.0f, 0.4f, SpeedRatio)
		: 0.0f;

	if (!Wheel.bIsGrounded || NormalLoad <= 0.0f || GroundedWheelCount == 0)
	{
		return;
	}

	const FQuat SteeringRotation(BodyCollision->GetUpVector(), FMath::DegreesToRadians(Wheel.SteeringAngle));
	const FVector WheelDirection = SteeringRotation.RotateVector(BodyCollision->GetForwardVector());
	const FVector Forward = FVector::VectorPlaneProject(WheelDirection, ContactNormals[WheelIndex]).GetSafeNormal();
	const FVector Right = FVector::CrossProduct(ContactNormals[WheelIndex], Forward).GetSafeNormal();
	if (Forward.IsNearlyZero() || Right.IsNearlyZero())
	{
		return;
	}

	const FVector PointVelocity = BodyCollision->GetPhysicsLinearVelocityAtPoint(ContactPoints[WheelIndex]);
	const float LongitudinalSpeed = FVector::DotProduct(PointVelocity, Forward);
	const float LateralSpeed = FVector::DotProduct(PointVelocity, Right);
	Wheel.SurfaceSpeed = LongitudinalSpeed;

	FVector TireForce = -Right * LateralSpeed * TireGrip;
	const float MaxStopForce = VehicleMass * FMath::Abs(LongitudinalSpeed)
		/ (FMath::Max(DeltaSeconds, KINDA_SMALL_NUMBER) * GroundedWheelCount);

	if (bBrakePressed)
	{
		const float BrakeMagnitude = FMath::Min(BrakeForce / GroundedWheelCount, MaxStopForce);
		TireForce -= Forward * FMath::Sign(LongitudinalSpeed) * BrakeMagnitude;
	}
	else if (FMath::Abs(Throttle) > KINDA_SMALL_NUMBER && WheelIndex >= 2)
	{
		const float SpeedLimit = Throttle > 0.0f ? MaxForwardSpeed : MaxReverseSpeed;
		const float RequestedDirectionSpeed = LongitudinalSpeed * FMath::Sign(Throttle);
		const float DriveScale = 1.0f - FMath::Clamp(RequestedDirectionSpeed / SpeedLimit, 0.0f, 1.0f);
		TireForce += Forward * Throttle * EngineForce * 0.5f * DriveScale;
	}
	else
	{
		const float Resistance = FMath::Clamp(
			-LongitudinalSpeed * RollingResistance,
			-MaxStopForce,
			MaxStopForce);
		TireForce += Forward * Resistance;
	}

	TireForce = TireForce.GetClampedToMaxSize(NormalLoad * TireFrictionLimit);
	BodyCollision->AddForceAtLocation(TireForce, ContactPoints[WheelIndex]);
}

void ABumperCarPawn::UpdateWheelVisual(int32 WheelIndex)
{
	if (!WheelMeshes[WheelIndex])
	{
		return;
	}

	const FBumperCarWheelState& Wheel = WheelStates[WheelIndex];
	WheelMeshes[WheelIndex]->SetRelativeLocation(
		GetWheelAnchorLocal(WheelIndex) - FVector(0.0f, 0.0f, Wheel.SuspensionLength));
	WheelMeshes[WheelIndex]->SetRelativeScale3D(FVector(
		WheelRadius / 50.0f,
		WheelRadius / 50.0f,
		WheelWidth / 100.0f));

	const FQuat SteeringRotation(FVector::UpVector, FMath::DegreesToRadians(Wheel.SteeringAngle));
	const FQuat RollingRotation(FVector::RightVector, FMath::DegreesToRadians(Wheel.SpinAngle));
	const FQuat CylinderRotation(FRotator(90.0f, 90.0f, 0.0f));
	WheelMeshes[WheelIndex]->SetRelativeRotation((SteeringRotation * RollingRotation * CylinderRotation).Rotator());
}

void ABumperCarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("BumperCar_Throttle"), this, &ABumperCarPawn::SetThrottle);
	PlayerInputComponent->BindAxis(TEXT("BumperCar_Steering"), this, &ABumperCarPawn::SetSteering);
	PlayerInputComponent->BindAction(TEXT("BumperCar_Brake"), IE_Pressed, this, &ABumperCarPawn::PressBrake);
	PlayerInputComponent->BindAction(TEXT("BumperCar_Brake"), IE_Released, this, &ABumperCarPawn::ReleaseBrake);
	PlayerInputComponent->BindAction(TEXT("BumperCar_Reset"), IE_Pressed, this, &ABumperCarPawn::ResetVehicle);
}

void ABumperCarPawn::UnPossessed()
{
	Super::UnPossessed();
	TargetThrottle = 0.0f;
	TargetSteering = 0.0f;
	bBrakePressed = false;
}

void ABumperCarPawn::SetThrottle(float Value)
{
	TargetThrottle = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ABumperCarPawn::SetSteering(float Value)
{
	TargetSteering = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ABumperCarPawn::PressBrake()
{
	bBrakePressed = true;
}

void ABumperCarPawn::ReleaseBrake()
{
	bBrakePressed = false;
}

void ABumperCarPawn::ResetVehicle()
{
	const FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 150.0f);
	const FRotator ResetRotation(0.0f, GetActorRotation().Yaw, 0.0f);
	SetActorLocationAndRotation(ResetLocation, ResetRotation, false, nullptr, ETeleportType::TeleportPhysics);
	BodyCollision->SetPhysicsLinearVelocity(FVector::ZeroVector);
	BodyCollision->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	Throttle = 0.0f;
	Steering = 0.0f;
}

float ABumperCarPawn::GetSpeedKmh() const
{
	return BodyCollision->GetPhysicsLinearVelocity().Size() * 0.036f;
}
