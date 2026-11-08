#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplaySubsystem.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UGameplaySubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override { return TStatId(); }

private:
	float TimeUntilNextCheck = 0.0f;
	bool bSpawnedDemoEnemies = false;

	UPROPERTY()
	TObjectPtr<class UGameplayHUDWidget> HudWidget;

	void ReturnControlToCharacter(APlayerController* Controller, class ABuggyPawn* Buggy);
	void SpawnDemoEnemies(APawn* PlayerPawn);
};
