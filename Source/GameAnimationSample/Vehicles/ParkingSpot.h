#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParkingSpot.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AParkingSpot : public AActor
{
	GENERATED_BODY()

public:
	AParkingSpot();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> Area;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Marker;
};
