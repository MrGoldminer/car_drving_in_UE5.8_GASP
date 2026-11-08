#include "GameAnimationSample/Weapons/MeleeWeapon.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AMeleeWeapon::AMeleeWeapon()
{
	Damage = 35.0f;
}

void AMeleeWeapon::BuildModel()
{
	switch (WeaponType)
	{
	case EWeaponType::TangDao:
		BuildTangDaoModel();
		break;
	case EWeaponType::Katana:
	default:
		BuildKatanaModel();
		break;
	}
}

void AMeleeWeapon::BuildKatanaModel()
{
	AddCylinderPart(TEXT("Tsuka"), FVector(-12.0f, 0.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f), FVector(4.0f, 4.0f, 26.0f));
	AddBoxPart(TEXT("Tsuba"), FVector(2.0f, 0.0f, 0.0f), FRotator::ZeroRotator, FVector(2.0f, 12.0f, 12.0f));
	AddBoxPart(TEXT("BladeBase"), FVector(20.0f, 0.0f, 0.0f), FRotator::ZeroRotator, FVector(34.0f, 0.8f, 3.5f));
	AddBoxPart(TEXT("BladeMid"), FVector(51.0f, 0.0f, 1.5f), FRotator(0.0f, -4.0f, 0.0f), FVector(30.0f, 0.7f, 3.2f));
	AddBoxPart(TEXT("BladeTip"), FVector(79.0f, 0.0f, 4.5f), FRotator(0.0f, -9.0f, 0.0f), FVector(26.0f, 0.5f, 2.8f));
}

void AMeleeWeapon::BuildTangDaoModel()
{
	AddCylinderPart(TEXT("Handle"), FVector(-10.0f, 0.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f), FVector(4.5f, 4.5f, 22.0f));
	AddBoxPart(TEXT("Guard"), FVector(2.0f, 0.0f, 0.0f), FRotator::ZeroRotator, FVector(2.5f, 10.0f, 8.0f));
	AddBoxPart(TEXT("Blade"), FVector(45.0f, 0.0f, 0.0f), FRotator::ZeroRotator, FVector(85.0f, 0.9f, 4.0f));
	AddBoxPart(TEXT("Tip"), FVector(89.0f, 0.0f, 1.0f), FRotator(0.0f, -25.0f, 0.0f), FVector(6.0f, 0.7f, 3.0f));
}

void AMeleeWeapon::StartAttack()
{
	if (bSwinging || CooldownRemaining > 0.0f || IsHidden())
	{
		return;
	}

	bSwinging = true;
	SwingTime = 0.0f;
	CooldownRemaining = AttackCooldown;
	HitVictims.Reset();
}

void AMeleeWeapon::StopAttack()
{
}

void AMeleeWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CooldownRemaining = FMath::Max(0.0f, CooldownRemaining - DeltaSeconds);

	if (!bSwinging)
	{
		return;
	}

	SwingTime += DeltaSeconds;
	const float Alpha = FMath::Clamp(SwingTime / SwingDuration, 0.0f, 1.0f);

	const float SwingPitch = FMath::Lerp(-75.0f, 65.0f, Alpha);
	const FQuat SwingQuat = FRotator(SwingPitch, -25.0f + 50.0f * Alpha, 0.0f).Quaternion();
	WeaponRoot->SetRelativeRotation((GripRotation.Quaternion() * SwingQuat).Rotator());

	if (Alpha > 0.15f && Alpha < 0.85f)
	{
		PerformMeleeTrace();
	}

	if (SwingTime >= SwingDuration)
	{
		bSwinging = false;
		WeaponRoot->SetRelativeRotation(GripRotation);
	}
}

void AMeleeWeapon::PerformMeleeTrace()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	FRotator Facing = OwnerPawn->GetActorRotation();
	if (AController* Controller = OwnerPawn->GetController())
	{
		Facing = Controller->GetControlRotation();
		Facing.Pitch = 0.0f;
		Facing.Roll = 0.0f;
	}

	const FVector Origin = OwnerPawn->GetActorLocation();
	const FVector Start = Origin + Facing.Vector() * 40.0f;
	const FVector End = Origin + Facing.Vector() * Reach;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MeleeWeapon), false, OwnerPawn);
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	const FCollisionShape Shape = FCollisionShape::MakeSphere(60.0f);
	GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, Shape, QueryParams);

	for (const FHitResult& Hit : Hits)
	{
		AActor* Victim = Hit.GetActor();
		if (!Victim || Victim == OwnerPawn || HitVictims.Contains(Victim))
		{
			continue;
		}

		HitVictims.Add(Victim);
		ApplyWeaponDamage(Victim);
	}
}
