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

protected:
	virtual void BuildModel() override;

private:
	bool bSwinging = false;
	float SwingTime = 0.0f;
	float CooldownRemaining = 0.0f;
	TArray<TWeakObjectPtr<AActor>> HitVictims;

	void PerformMeleeTrace();
	void BuildKatanaModel();
	void BuildTangDaoModel();
};
