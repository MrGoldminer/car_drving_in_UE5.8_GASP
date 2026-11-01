#include "GameAnimationSample/Character/GrapplingHookComponent.h"

#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "MoverExamplesCharacter.h"
#include "UObject/ConstructorHelpers.h"

UGrapplingHookComponent::UGrapplingHookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrapplingHookComponent::BeginPlay()
{
	Super::BeginPlay();

	RopeMesh = NewObject<USplineMeshComponent>(GetOwner(), TEXT("GrappleRope"));
	RopeMesh->RegisterComponent();
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RopeMesh->SetWorldLocation(FVector::ZeroVector);
	RopeMesh->SetRelativeScale3D(FVector(0.015f, 0.015f, 1.0f));
	RopeMesh->SetVisibility(false);

	if (UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder")))
	{
		RopeMesh->SetStaticMesh(CylinderMesh);
	}
}

void UGrapplingHookComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	if (!bInputBound && OwnerPawn->InputComponent)
	{
		OwnerPawn->InputComponent->BindAction(TEXT("Grapple"), IE_Pressed, this, &UGrapplingHookComponent::StartGrapple);
		OwnerPawn->InputComponent->BindAction(TEXT("Grapple"), IE_Released, this, &UGrapplingHookComponent::ReleaseGrapple);
		bInputBound = true;
	}

	if (!bIsGrappling)
	{
		return;
	}

	if (!OwnerPawn->GetController() || !HookComponent.IsValid())
	{
		ReleaseGrapple();
		return;
	}

	HookPoint = HookComponent->GetComponentTransform().TransformPosition(HookLocalPoint);
	const FVector Direction = HookPoint - OwnerPawn->GetActorLocation();
	if (Direction.Size() <= StopDistance)
	{
		ReleaseGrapple();
		return;
	}

	const FVector PullVelocity = Direction.GetSafeNormal() * PullSpeed;
	if (AMoverExamplesCharacter* MoverCharacter = Cast<AMoverExamplesCharacter>(OwnerPawn))
	{
		MoverCharacter->RequestMoveByVelocity(PullVelocity);
	}
	else if (ACharacter* Character = Cast<ACharacter>(OwnerPawn))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->SetMovementMode(MOVE_Falling);
			Movement->Velocity = FMath::VInterpTo(Movement->Velocity, PullVelocity, DeltaTime, PullResponsiveness);
		}
	}

	UpdateRope();
}

void UGrapplingHookComponent::StartGrapple()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* Controller = Cast<APlayerController>(OwnerPawn->GetController());
	if (!Controller || !Controller->PlayerCameraManager)
	{
		return;
	}

	const FVector TraceStart = Controller->PlayerCameraManager->GetCameraLocation();
	const FVector TraceEnd = TraceStart + Controller->PlayerCameraManager->GetCameraRotation().Vector() * MaxDistance;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(GrappleTrace), false, OwnerPawn);
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		return;
	}

	HookComponent = Hit.GetComponent();
	if (!HookComponent.IsValid())
	{
		return;
	}

	HookLocalPoint = HookComponent->GetComponentTransform().InverseTransformPosition(Hit.Location);
	HookPoint = Hit.Location;
	bIsGrappling = true;
	if (RopeMesh)
	{
		RopeMesh->SetVisibility(true);
	}
	UpdateRope();
}

void UGrapplingHookComponent::ReleaseGrapple()
{
	bIsGrappling = false;
	HookComponent.Reset();
	if (RopeMesh)
	{
		RopeMesh->SetVisibility(false);
	}
	if (AMoverExamplesCharacter* MoverCharacter = Cast<AMoverExamplesCharacter>(GetOwner()))
	{
		MoverCharacter->RequestMoveByVelocity(FVector::ZeroVector);
	}
}

void UGrapplingHookComponent::UpdateRope()
{
	if (!RopeMesh)
	{
		return;
	}

	FVector RopeStart = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, 60.0f);
	if (USkeletalMeshComponent* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (Mesh->DoesSocketExist(TEXT("hand_r")))
		{
			RopeStart = Mesh->GetSocketLocation(TEXT("hand_r"));
		}
	}

	const FVector Delta = HookPoint - RopeStart;
	RopeMesh->SetStartAndEnd(RopeStart, Delta, HookPoint, Delta);
}
