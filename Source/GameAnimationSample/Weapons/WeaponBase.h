#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameAnimationSample/Weapons/WeaponTypes.h"
#include "WeaponBase.generated.h"

class UMaterialInterface;
class USceneComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	virtual void Tick(float DeltaSeconds) override;

	virtual void StartAttack() {}
	virtual void StopAttack() {}

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponType GetWeaponType() const { return WeaponType; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void RebuildModel();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Damage = 20.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> WeaponRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FVector GripLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FRotator GripRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Model")
	FSoftObjectPath ModelAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Model")
	FVector ModelScale = FVector(1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Model")
	FRotator ModelRotation = FRotator::ZeroRotator;

protected:
	virtual void BuildModel() {}

	bool TryBuildFromAsset();

	UStaticMeshComponent* AddBoxPart(FName Name, FVector Location, FRotator Rotation, FVector Size);
	UStaticMeshComponent* AddCylinderPart(FName Name, FVector Location, FRotator Rotation, FVector Size);

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> PartMaterial;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> ModelParts;

	void ApplyWeaponDamage(AActor* Victim);
};
