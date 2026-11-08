#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Weapons/WeaponBase.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API AMeleeWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AMeleeWeapon();

	virtual void StartAttack() override;
	virtual void StopAttack() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	float Reach = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	float SwingDuration = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	float AttackCooldown = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Trail")
	bool bEnableTrail = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Trail")
	TObjectPtr<UMaterialInterface> TrailMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Trail")
	FVector BladeRootOffset = FVector(5.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Trail")
	FVector BladeTipOffset = FVector(95.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

protected:
	virtual void BuildModel() override;

private:
	struct FTrailPoint
	{
		FVector Root = FVector::ZeroVector;
		FVector Tip = FVector::ZeroVector;
		float Age = 0.0f;
	};

	bool bSwinging = false;
	float SwingTime = 0.0f;
	float CooldownRemaining = 0.0f;
	TArray<TWeakObjectPtr<AActor>> HitVictims;
	TArray<FTrailPoint> TrailPoints;

	UPROPERTY()
	TObjectPtr<class UProceduralMeshComponent> TrailMesh;

	void PerformMeleeTrace();
	void BuildKatanaModel();
	void BuildTangDaoModel();
	void UpdateTrail(float DeltaSeconds);
	void RebuildTrailMesh();
};
