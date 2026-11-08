#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameAnimationSample/Weapons/WeaponTypes.h"
#include "WeaponManagerComponent.generated.h"

class AWeaponBase;
class AMeleeWeapon;
class ARifleWeapon;
class UInputComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponManagerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void EquipWeapon(EWeaponType NewType);

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void CycleWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapons")
	EWeaponType GetCurrentWeaponType() const { return CurrentType; }

	UFUNCTION(BlueprintPure, Category = "Weapons")
	AWeaponBase* GetCurrentWeapon() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	FName HandSocket = TEXT("hand_r");

private:
	UPROPERTY()
	TObjectPtr<AMeleeWeapon> Katana;

	UPROPERTY()
	TObjectPtr<AMeleeWeapon> TangDao;

	UPROPERTY()
	TObjectPtr<ARifleWeapon> Rifle;

	EWeaponType CurrentType = EWeaponType::None;
	bool bInputBound = false;

	void BindInput(UInputComponent* InputComponent);
	void DisableNextPawnTabMapping();
	void StartAttackInput();
	void StopAttackInput();
	AWeaponBase* FindWeapon(EWeaponType Type) const;
};
