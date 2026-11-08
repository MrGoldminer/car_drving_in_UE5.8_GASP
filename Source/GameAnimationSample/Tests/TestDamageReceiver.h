#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestDamageReceiver.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API ATestDamageReceiver : public ACharacter
{
	GENERATED_BODY()

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override
	{
		AccumulatedDamage += DamageAmount;
		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}

	float AccumulatedDamage = 0.0f;
};
