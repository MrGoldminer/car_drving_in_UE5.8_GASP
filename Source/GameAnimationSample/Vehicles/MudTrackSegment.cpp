#include "GameAnimationSample/Vehicles/MudTrackSegment.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AMudTrackSegment::AMudTrackSegment()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionProfileName(TEXT("BlockAll"));

	Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
	Visual->SetupAttachment(Collision);
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeAsset.Succeeded())
	{
		Visual->SetStaticMesh(CubeAsset.Object);
	}
}

void AMudTrackSegment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Collision->SetBoxExtent(FullExtent * 0.5f);
	Collision->SetPhysMaterialOverride(SurfacePhysicalMaterial);
	Visual->SetRelativeScale3D(FullExtent / 100.0f);
	Visual->SetMaterial(0, SurfaceMaterial);
}
