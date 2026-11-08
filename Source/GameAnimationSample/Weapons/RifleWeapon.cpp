#include "GameAnimationSample/Weapons/RifleWeapon.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

ARifleWeapon::ARifleWeapon()
{
	WeaponType = EWeaponType::AK47;
	Damage = 25.0f;

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(WeaponRoot);
	MuzzlePoint->SetRelativeLocation(FVector(48.0f, 0.0f, 2.0f));

	MuzzleFlash = CreateDefaultSubobject<UPointLightComponent>(TEXT("MuzzleFlash"));
	MuzzleFlash->SetupAttachment(MuzzlePoint);
	MuzzleFlash->SetLightColor(FLinearColor(1.0f, 0.65f, 0.2f));
	MuzzleFlash->SetIntensity(3000.0f);
	MuzzleFlash->SetAttenuationRadius(300.0f);
	MuzzleFlash->SetVisibility(false);
}

void ARifleWeapon::BuildModel()
{
	AddBoxPart(TEXT("Receiver"), FVector(0.0f, 0.0f, 0.0f), FRotator::ZeroRotator, FVector(35.0f, 7.0f, 9.0f));
	AddBoxPart(TEXT("Handguard"), FVector(20.0f, 0.0f, 0.5f), FRotator::ZeroRotator, FVector(18.0f, 8.0f, 8.0f));
	AddCylinderPart(TEXT("Barrel"), FVector(36.0f, 0.0f, 2.0f), FRotator(0.0f, 90.0f, 0.0f), FVector(3.0f, 3.0f, 22.0f));
	AddBoxPart(TEXT("FrontSight"), FVector(44.0f, 0.0f, 6.0f), FRotator::ZeroRotator, FVector(2.0f, 2.0f, 6.0f));
	AddBoxPart(TEXT("Magazine"), FVector(6.0f, 0.0f, -11.0f), FRotator(0.0f, 20.0f, 0.0f), FVector(8.0f, 6.0f, 18.0f));
	AddBoxPart(TEXT("Stock"), FVector(-27.0f, 0.0f, -2.0f), FRotator::ZeroRotator, FVector(22.0f, 6.0f, 10.0f));
	AddBoxPart(TEXT("Grip"), FVector(-8.0f, 0.0f, -10.0f), FRotator(0.0f, -20.0f, 0.0f), FVector(5.0f, 5.0f, 12.0f));
}

void ARifleWeapon::StartAttack()
{
	if (IsHidden())
	{
		return;
	}
	bFiring = true;
}

void ARifleWeapon::StopAttack()
{
	bFiring = false;
}

void ARifleWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TimeUntilNextShot -= DeltaSeconds;
	if (bFiring && !IsHidden() && TimeUntilNextShot <= 0.0f)
	{
		FireOnce();
		TimeUntilNextShot = 60.0f / FMath::Max(RoundsPerMinute, 1.0f);
	}

	if (MuzzleFlashRemaining > 0.0f)
	{
		MuzzleFlashRemaining -= DeltaSeconds;
		if (MuzzleFlashRemaining <= 0.0f)
		{
			MuzzleFlash->SetVisibility(false);
		}
	}

	UpdateTracers(DeltaSeconds);
}

void ARifleWeapon::FireOnce()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !GetWorld())
	{
		return;
	}

	FVector TraceStart = OwnerPawn->GetPawnViewLocation();
	FRotator ViewRotation = OwnerPawn->GetActorRotation();
	if (APlayerController* Controller = Cast<APlayerController>(OwnerPawn->GetController()))
	{
		if (Controller->PlayerCameraManager)
		{
			TraceStart = Controller->PlayerCameraManager->GetCameraLocation();
			ViewRotation = Controller->PlayerCameraManager->GetCameraRotation();
		}
	}
	else if (AController* OwnerController = OwnerPawn->GetController())
	{
		ViewRotation = OwnerController->GetControlRotation();
	}

	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * Range;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RifleWeapon), false, OwnerPawn);
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	GetWorld()->LineTraceMultiByChannel(Hits, TraceStart, TraceEnd, ECC_Pawn, QueryParams);

	FHitResult BestHit;
	bool bHasHit = false;
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == OwnerPawn || HitActor == this)
		{
			continue;
		}
		BestHit = Hit;
		bHasHit = true;
		break;
	}

	const FVector ImpactPoint = bHasHit ? BestHit.ImpactPoint : TraceEnd;
	if (bHasHit)
	{
		ApplyWeaponDamage(BestHit.GetActor());
	}

	SpawnTracer(MuzzlePoint->GetComponentLocation(), ImpactPoint);

	MuzzleFlash->SetVisibility(true);
	MuzzleFlashRemaining = 0.04f;

	PlayActionMontage(FireMontage);
}

void ARifleWeapon::SpawnTracer(const FVector& Start, const FVector& End)
{
	const FVector Delta = End - Start;
	const float Length = Delta.Size();
	if (Length < 1.0f || !CubeMesh)
	{
		return;
	}

	UStaticMeshComponent* TracerMesh = NewObject<UStaticMeshComponent>(this);
	TracerMesh->SetStaticMesh(CubeMesh);
	TracerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (TracerMaterial)
	{
		TracerMesh->SetMaterial(0, TracerMaterial);
	}
	else if (PartMaterial)
	{
		TracerMesh->SetMaterial(0, PartMaterial);
	}
	TracerMesh->RegisterComponent();

	TracerMesh->SetWorldLocation((Start + End) * 0.5f);
	TracerMesh->SetWorldRotation(Delta.Rotation());
	TracerMesh->SetWorldScale3D(FVector(Length / 100.0f, 0.015f, 0.015f));

	FTracer Tracer;
	Tracer.Mesh = TracerMesh;
	Tracer.RemainingLife = TracerLifetime;
	Tracers.Add(Tracer);
}

void ARifleWeapon::UpdateTracers(float DeltaSeconds)
{
	for (int32 Index = Tracers.Num() - 1; Index >= 0; --Index)
	{
		Tracers[Index].RemainingLife -= DeltaSeconds;
		if (Tracers[Index].RemainingLife <= 0.0f)
		{
			if (Tracers[Index].Mesh)
			{
				Tracers[Index].Mesh->DestroyComponent();
			}
			Tracers.RemoveAt(Index);
		}
	}
}
