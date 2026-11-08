#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Weapons/WeaponBase.h"
#include "RifleWeapon.generated.h"

class UPointLightComponent;
class USceneComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API ARifleWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ARifleWeapon();

	virtual void StartAttack() override;
	virtual void StopAttack() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	float RoundsPerMinute = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	float Range = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	float TracerLifetime = 0.06f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	TObjectPtr<UMaterialInterface> TracerMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MuzzlePoint;

protected:
	virtual void BuildModel() override;

private:
	struct FTracer
	{
		TObjectPtr<UStaticMeshComponent> Mesh;
		float RemainingLife = 0.0f;
	};

	bool bFiring = false;
	float TimeUntilNextShot = 0.0f;
	float MuzzleFlashRemaining = 0.0f;
	TArray<FTracer> Tracers;

	UPROPERTY()
	TObjectPtr<UPointLightComponent> MuzzleFlash;

	void FireOnce();
	void SpawnTracer(const FVector& Start, const FVector& End);
	void UpdateTracers(float DeltaSeconds);
};
