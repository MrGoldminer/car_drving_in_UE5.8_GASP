#include "GameAnimationSample/Weapons/WeaponBase.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	SetRootComponent(WeaponRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
	CubeMesh = CubeAsset.Succeeded() ? CubeAsset.Object : nullptr;
	CylinderMesh = CylinderAsset.Succeeded() ? CylinderAsset.Object : nullptr;
	PartMaterial = MaterialAsset.Succeeded() ? MaterialAsset.Object : nullptr;

	SetWeaponVisible(false);
}

void AWeaponBase::RebuildModel()
{
	for (UStaticMeshComponent* Part : ModelParts)
	{
		if (Part)
		{
			Part->DestroyComponent();
		}
	}
	ModelParts.Reset();
	if (TryBuildFromAsset())
	{
		return;
	}
	BuildModel();
}

bool AWeaponBase::TryBuildFromAsset()
{
	if (ModelAsset.IsNull())
	{
		return false;
	}

	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *ModelAsset.ToString());
	if (!Mesh)
	{
		return false;
	}

	UStaticMeshComponent* ModelPart = NewObject<UStaticMeshComponent>(this, TEXT("ImportedModel"));
	ModelPart->SetStaticMesh(Mesh);
	ModelPart->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ModelPart->RegisterComponent();
	ModelPart->AttachToComponent(WeaponRoot, FAttachmentTransformRules::KeepRelativeTransform);
	ModelPart->SetRelativeScale3D(ModelScale);
	ModelPart->SetRelativeRotation(ModelRotation);
	ModelParts.Add(ModelPart);
	return true;
}

void AWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

UStaticMeshComponent* AWeaponBase::AddBoxPart(FName Name, FVector Location, FRotator Rotation, FVector Size)
{
	UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(this, Name);
	Part->SetStaticMesh(CubeMesh);
	Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Part->RegisterComponent();
	Part->AttachToComponent(WeaponRoot, FAttachmentTransformRules::KeepRelativeTransform);
	Part->SetRelativeLocation(Location);
	Part->SetRelativeRotation(Rotation);
	Part->SetRelativeScale3D(Size / 100.0f);
	if (PartMaterial)
	{
		Part->SetMaterial(0, PartMaterial);
	}
	ModelParts.Add(Part);
	return Part;
}

UStaticMeshComponent* AWeaponBase::AddCylinderPart(FName Name, FVector Location, FRotator Rotation, FVector Size)
{
	UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(this, Name);
	Part->SetStaticMesh(CylinderMesh);
	Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Part->RegisterComponent();
	Part->AttachToComponent(WeaponRoot, FAttachmentTransformRules::KeepRelativeTransform);
	Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Part->SetRelativeLocation(Location);
	Part->SetRelativeRotation(Rotation);
	Part->SetRelativeScale3D(Size / 100.0f);
	if (PartMaterial)
	{
		Part->SetMaterial(0, PartMaterial);
	}
	ModelParts.Add(Part);
	return Part;
}

void AWeaponBase::SetWeaponVisible(bool bVisible)
{
	SetActorHiddenInGame(!bVisible);
	SetActorEnableCollision(false);
}

void AWeaponBase::ApplyWeaponDamage(AActor* Victim)
{
	if (!Victim || Victim == GetOwner())
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AController* InstigatorController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	UGameplayStatics::ApplyDamage(Victim, Damage, InstigatorController, this, UDamageType::StaticClass());
}

void AWeaponBase::PlayActionMontage(UAnimMontage* Montage)
{
	if (!Montage)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Play(Montage);
			}
		}
	}
}
