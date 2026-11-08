#include "GameAnimationSample/Character/EnemyTarget.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameAnimationSample/Character/HealthComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AEnemyTarget::AEnemyTarget()
{
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GrayMaterial(TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -10.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.55f, 0.35f, 1.2f));

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetCapsuleComponent());
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 62.0f));
	HeadMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));

	if (CubeAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeAsset.Object);
		HeadMesh->SetStaticMesh(CubeAsset.Object);
	}
	if (GrayMaterial.Succeeded())
	{
		BodyMesh->SetMaterial(0, GrayMaterial.Object);
		HeadMesh->SetMaterial(0, GrayMaterial.Object);
	}
}

void AEnemyTarget::BeginPlay()
{
	Super::BeginPlay();
	Health->OnDeath.AddDynamic(this, &AEnemyTarget::HandleDeath);
}

void AEnemyTarget::HandleDeath()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}
