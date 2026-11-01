#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BumperCarPawn.generated.h"

class UBoxComponent;
class UCameraComponent;
class UPhysicalMaterial;
class USpringArmComponent;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FBumperCarWheelState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wheel")
	bool bIsGrounded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wheel")
	float SuspensionLength = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wheel")
	float Compression = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wheel")
	float SteeringAngle = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wheel")
	float SpinAngle = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wheel")
	float SurfaceSpeed = 0.0f;
};

UCLASS(Blueprintable)
class GAMEANIMATIONSAMPLE_API ABumperCarPawn : public APawn
{
	GENERATED_BODY()

public:
	ABumperCarPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void UnPossessed() override;

	UFUNCTION(BlueprintCallable, Category = "Bumper Car")
	void ResetVehicle();

	UFUNCTION(BlueprintPure, Category = "Bumper Car")
	float GetSpeedKmh() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BodyCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SeatMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bumper Car|Wheels", meta = (EditFixedSize))
	TArray<FBumperCarWheelState> WheelStates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Physics", meta = (ClampMin = "100.0"))
	float VehicleMass = 1100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Driving", meta = (ClampMin = "0.0"))
	float EngineForce = 800000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Driving", meta = (ClampMin = "0.0"))
	float BrakeForce = 1000000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Driving", meta = (ClampMin = "0.0", Units = "cm/s"))
	float MaxForwardSpeed = 1600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Driving", meta = (ClampMin = "0.0", Units = "cm/s"))
	float MaxReverseSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Driving", meta = (ClampMin = "0.0", ClampMax = "60.0"))
	float MaxSteeringAngle = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Driving", meta = (ClampMin = "0.1"))
	float SteeringResponse = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Driving", meta = (ClampMin = "0.1"))
	float ThrottleResponse = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Suspension", meta = (ClampMin = "1.0", Units = "cm"))
	float SuspensionRestLength = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Suspension", meta = (ClampMin = "0.0", Units = "cm"))
	float SuspensionTravel = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Suspension", meta = (ClampMin = "0.0"))
	float SpringStiffness = 45000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Suspension", meta = (ClampMin = "0.0"))
	float SpringDamping = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Suspension", meta = (ClampMin = "0.0"))
	float MaxSuspensionForce = 500000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Tires", meta = (ClampMin = "0.0"))
	float TireGrip = 5500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Tires", meta = (ClampMin = "0.0"))
	float TireFrictionLimit = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Tires", meta = (ClampMin = "0.0"))
	float RollingResistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Wheels", meta = (ClampMin = "1.0", Units = "cm"))
	float WheelRadius = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Wheels", meta = (ClampMin = "1.0", Units = "cm"))
	float WheelWidth = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Wheels", meta = (ClampMin = "1.0", Units = "cm"))
	float WheelBase = 240.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Wheels", meta = (ClampMin = "1.0", Units = "cm"))
	float TrackWidth = 170.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bumper Car|Debug")
	bool bDrawSuspensionDebug = false;

protected:
	static constexpr int32 NumWheels = 4;

	UPROPERTY()
	TObjectPtr<UPhysicalMaterial> BumperMaterial;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> WheelMeshes[NumWheels];

	FVector ContactPoints[NumWheels] = {};
	FVector ContactNormals[NumWheels] = {};
	EPhysicalSurface ContactSurfaces[NumWheels] = {};

	float TargetThrottle = 0.0f;
	float TargetSteering = 0.0f;
	float Throttle = 0.0f;
	float Steering = 0.0f;
	bool bBrakePressed = false;

	FVector GetWheelAnchorLocal(int32 WheelIndex) const;
	float SimulateSuspension(int32 WheelIndex);
	void ApplyTireForces(int32 WheelIndex, float NormalLoad, int32 GroundedWheelCount, float DeltaSeconds);
	virtual void UpdateWheelVisual(int32 WheelIndex);

	UFUNCTION(BlueprintCallable, Category = "Bumper Car|Input")
	void SetThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "Bumper Car|Input")
	void SetSteering(float Value);

	UFUNCTION(BlueprintCallable, Category = "Bumper Car|Input")
	void PressBrake();

	UFUNCTION(BlueprintCallable, Category = "Bumper Car|Input")
	void ReleaseBrake();
};
