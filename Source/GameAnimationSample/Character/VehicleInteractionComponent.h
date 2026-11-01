#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleInteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UVehicleInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVehicleInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Vehicle Interaction")
	void TryEnterVehicle();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle Interaction", meta = (ClampMin = "50.0", Units = "cm"))
	float InteractionDistance = 300.0f;

private:
	bool bInputBound = false;
};
