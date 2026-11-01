#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Vehicles/BumperCarPawn.h"
#include "BuggyPawn.generated.h"

class UAnimMontage;
class AController;
class USceneComponent;

UCLASS(Blueprintable)
class GAMEANIMATIONSAMPLE_API ABuggyPawn : public ABumperCarPawn
{
	GENERATED_BODY()

public:
	ABuggyPawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Buggy|Driver")
	bool EnterVehicle(APawn* Character);

	UFUNCTION(BlueprintCallable, Category = "Buggy|Driver")
	bool ExitVehicle();

	UFUNCTION(BlueprintPure, Category = "Buggy|Driver")
	bool HasDriver() const { return bHasDriver; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SeatPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> ExitPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buggy|Driver")
	bool bHasDriver = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buggy|Driver")
	bool bHideDriver = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buggy|Driver")
	TObjectPtr<UAnimMontage> DriverAnimation;

protected:
	virtual void UpdateWheelVisual(int32 WheelIndex) override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> ModelParts;

	UPROPERTY()
	TObjectPtr<APawn> DriverPawn;

	UPROPERTY()
	TObjectPtr<AController> DriverController;

	bool bDriverTickEnabled = true;
	bool bDriverCollisionEnabled = true;
	FVector WheelSourceCenters[NumWheels] = {};

	static FVector MapModelPoint(const FVector& Point);
	void ExitVehicleInput() { ExitVehicle(); }
};
