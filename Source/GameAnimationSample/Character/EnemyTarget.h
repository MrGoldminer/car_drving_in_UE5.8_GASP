#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyTarget.generated.h"

class UHealthComponent;
class UStaticMeshComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AEnemyTarget : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> Health;

	UFUNCTION()
	void HandleDeath();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> HeadMesh;
};
