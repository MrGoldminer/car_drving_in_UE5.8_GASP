#include "GameAnimationSample/Character/GameplaySubsystem.h"

#include "GameAnimationSample/Character/GrapplingHookComponent.h"
#include "GameAnimationSample/Character/VehicleInteractionComponent.h"
#include "GameAnimationSample/Vehicles/BumperCarPawn.h"
#include "GameAnimationSample/Weapons/WeaponManagerComponent.h"
#include "GameFramework/PlayerController.h"

void UGameplaySubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeUntilNextCheck -= DeltaTime;
	if (TimeUntilNextCheck > 0.0f)
	{
		return;
	}
	TimeUntilNextCheck = 0.5f;

	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = Iterator->Get();
		APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
		if (!Pawn || Pawn->IsA<ABumperCarPawn>())
		{
			continue;
		}

		if (!Pawn->FindComponentByClass<UGrapplingHookComponent>())
		{
			UGrapplingHookComponent* Grapple = NewObject<UGrapplingHookComponent>(Pawn, TEXT("GrapplingHook"));
			Grapple->RegisterComponent();
		}
		if (!Pawn->FindComponentByClass<UVehicleInteractionComponent>())
		{
			UVehicleInteractionComponent* Interaction = NewObject<UVehicleInteractionComponent>(Pawn, TEXT("VehicleInteraction"));
			Interaction->RegisterComponent();
		}
		if (!Pawn->FindComponentByClass<UWeaponManagerComponent>())
		{
			UWeaponManagerComponent* Weapons = NewObject<UWeaponManagerComponent>(Pawn, TEXT("WeaponManager"));
			Weapons->RegisterComponent();
		}
	}
}
