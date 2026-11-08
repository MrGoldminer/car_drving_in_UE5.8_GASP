#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Vehicles/BumperCarPawn.h"
#include "BuggyPawn.generated.h"

class AParkingSpot;
class UAnimMontage;
class AController;
class UInstancedStaticMeshComponent;
class USceneComponent;

UCLASS(Blueprintable)
class GAMEANIMATIONSAMPLE_API ABuggyPawn : public ABumperCarPawn
{
	GENERATED_BODY()

public:
	ABuggyPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Buggy|Driver")
	bool EnterVehicle(APawn* Character);

	UFUNCTION(BlueprintCallable, Category = "Buggy|Driver")
	bool ExitVehicle();

	UFUNCTION(BlueprintPure, Category = "Buggy|Driver")
	bool HasDriver() const { return bHasDriver; }

	UFUNCTION(BlueprintCallable, Category = "Buggy|Driving")
	void ToggleAutoPark();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buggy|Driving")
	bool bAutoParkEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SeatPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> ExitPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> SplashMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buggy|Driver")
	bool bHasDriver = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buggy|Driver")
	bool bHideDriver = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buggy|Driver")
	TObjectPtr<UAnimMontage> DriverAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buggy|Effects")
	bool bEnableWaterSplash = true;

private:
	struct FDroplet
	{
		FVector Position = FVector::ZeroVector;
		FVector Velocity = FVector::ZeroVector;
		float Lifetime = 0.0f;
		float Scale = 1.0f;
	};

	UPROPERTY()
	TObjectPtr<APawn> DriverPawn;

	UPROPERTY()
	TObjectPtr<AController> DriverController;

	TArray<FDroplet> Droplets;
	bool bDriverTickEnabled = true;
	bool bDriverCollisionEnabled = true;
	TWeakObjectPtr<AParkingSpot> AutoParkTarget;

	void UpdateSplashes(float DeltaSeconds);
	void UpdateAutoPark();
	void ExitVehicleInput() { ExitVehicle(); }
};
