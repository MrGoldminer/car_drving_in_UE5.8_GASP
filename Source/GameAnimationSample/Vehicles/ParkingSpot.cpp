#include "GameAnimationSample/Vehicles/ParkingSpot.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AParkingSpot::AParkingSpot()
{
	Area = CreateDefaultSubobject<UBoxComponent>(TEXT("Area"));
	SetRootComponent(Area);
	Area->InitBoxExtent(FVector(90.0f, 160.0f, 60.0f));
	Area->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Marker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Marker"));
	Marker->SetupAttachment(Area);
	Marker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Marker->SetRelativeScale3D(FVector(1.8f, 3.2f, 0.04f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeAsset.Succeeded())
	{
		Marker->SetStaticMesh(CubeAsset.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MarkerMaterial(TEXT("/Game/Vehicles/Effects/M_ParkingSpot.M_ParkingSpot"));
	if (MarkerMaterial.Succeeded())
	{
		Marker->SetMaterial(0, MarkerMaterial.Object);
	}
}
