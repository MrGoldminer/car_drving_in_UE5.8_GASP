#include "GameAnimationSample/Weapons/WeaponManagerComponent.h"

#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameAnimationSample/Weapons/MeleeWeapon.h"
#include "GameAnimationSample/Weapons/RifleWeapon.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

UWeaponManagerComponent::UWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Katana = GetWorld()->SpawnActor<AMeleeWeapon>(SpawnParams);
	Katana->WeaponType = EWeaponType::Katana;
	Katana->RebuildModel();

	TangDao = GetWorld()->SpawnActor<AMeleeWeapon>(SpawnParams);
	TangDao->WeaponType = EWeaponType::TangDao;
	TangDao->RebuildModel();

	Rifle = GetWorld()->SpawnActor<ARifleWeapon>(SpawnParams);
	Rifle->RebuildModel();
}

void UWeaponManagerComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!bInputBound && OwnerPawn && OwnerPawn->InputComponent)
	{
		BindInput(OwnerPawn->InputComponent);
		bInputBound = true;
	}
}

void UWeaponManagerComponent::BindInput(UInputComponent* InputComponent)
{
	DisableNextPawnTabMapping();
	InputComponent->BindAction(TEXT("WeaponCycle"), IE_Pressed, this, &UWeaponManagerComponent::CycleWeapon);
	InputComponent->BindAction(TEXT("WeaponAttack"), IE_Pressed, this, &UWeaponManagerComponent::StartAttackInput);
	InputComponent->BindAction(TEXT("WeaponAttack"), IE_Released, this, &UWeaponManagerComponent::StopAttackInput);
}

void UWeaponManagerComponent::DisableNextPawnTabMapping()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* Controller = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	if (!Controller)
	{
		return;
	}

	UInputMappingContext* Sandbox = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/IMC_Sandbox.IMC_Sandbox"));
	UInputAction* NextPawn = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_NextPawn.IA_NextPawn"));
	if (!Sandbox || !NextPawn)
	{
		return;
	}

	Sandbox->UnmapKey(NextPawn, EKeys::Tab);
	if (ULocalPlayer* LocalPlayer = Controller->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->RequestRebuildControlMappings();
		}
	}
}

AWeaponBase* UWeaponManagerComponent::FindWeapon(EWeaponType Type) const
{
	switch (Type)
	{
	case EWeaponType::Katana:
		return Katana;
	case EWeaponType::TangDao:
		return TangDao;
	case EWeaponType::AK47:
		return Rifle;
	default:
		return nullptr;
	}
}

AWeaponBase* UWeaponManagerComponent::GetCurrentWeapon() const
{
	return FindWeapon(CurrentType);
}

void UWeaponManagerComponent::EquipWeapon(EWeaponType NewType)
{
	if (CurrentType == NewType)
	{
		return;
	}

	if (AWeaponBase* Previous = GetCurrentWeapon())
	{
		Previous->StopAttack();
		Previous->SetWeaponVisible(false);
		Previous->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	CurrentType = NewType;

	AWeaponBase* Next = GetCurrentWeapon();
	if (!Next)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->GetMesh())
	{
		Next->AttachToComponent(
			Character->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			HandSocket);
	}
	else
	{
		Next->AttachToActor(GetOwner(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

	Next->SetActorRelativeLocation(Next->GripLocation);
	Next->SetActorRelativeRotation(Next->GripRotation);
	Next->SetWeaponVisible(true);
}

void UWeaponManagerComponent::CycleWeapon()
{
	const uint8 NextIndex = (static_cast<uint8>(CurrentType) + 1) % 4;
	EquipWeapon(static_cast<EWeaponType>(NextIndex));
}

void UWeaponManagerComponent::StartAttackInput()
{
	if (AWeaponBase* Weapon = GetCurrentWeapon())
	{
		Weapon->StartAttack();
	}
}

void UWeaponManagerComponent::StopAttackInput()
{
	if (AWeaponBase* Weapon = GetCurrentWeapon())
	{
		Weapon->StopAttack();
	}
}
