#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MudTrackSegment.generated.h"

class UBoxComponent;
class UMaterialInterface;
class UPhysicalMaterial;
class UStaticMeshComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AMudTrackSegment : public AActor
{
	GENERATED_BODY()

public:
	AMudTrackSegment();
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Visual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
	FVector FullExtent = FVector(405.0f, 700.0f, 20.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
	TObjectPtr<UMaterialInterface> SurfaceMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
	TObjectPtr<UPhysicalMaterial> SurfacePhysicalMaterial;
};
