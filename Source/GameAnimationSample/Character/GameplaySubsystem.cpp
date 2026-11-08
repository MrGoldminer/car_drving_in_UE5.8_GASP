#include "GameAnimationSample/Character/GameplaySubsystem.h"

#include "EngineUtils.h"
#include "GameAnimationSample/Character/EnemyTarget.h"
#include "GameAnimationSample/Character/GrapplingHookComponent.h"
#include "GameAnimationSample/Character/HealthComponent.h"
#include "GameAnimationSample/Character/VehicleInteractionComponent.h"
#include "GameAnimationSample/UI/GameplayHUDWidget.h"
#include "GameAnimationSample/Vehicles/BuggyPawn.h"
#include "GameAnimationSample/Vehicles/BumperCarPawn.h"
#include "GameAnimationSample/Weapons/WeaponManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
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
		if (!Pawn)
		{
			continue;
		}

		if (ABuggyPawn* Buggy = Cast<ABuggyPawn>(Pawn))
		{
			if (!Buggy->HasDriver())
			{
				ReturnControlToCharacter(Controller, Buggy);
			}
			continue;
		}
		if (Pawn->IsA<ABumperCarPawn>())
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
		if (!Pawn->FindComponentByClass<UHealthComponent>())
		{
			UHealthComponent* Health = NewObject<UHealthComponent>(Pawn, TEXT("Health"));
			Health->RegisterComponent();
		}

		if (Controller->IsLocalPlayerController() && !HudWidget)
		{
			HudWidget = CreateWidget<UGameplayHUDWidget>(Controller);
			if (HudWidget)
			{
				HudWidget->AddToViewport();
			}
		}

		SpawnDemoEnemies(Pawn);
	}
}

void UGameplaySubsystem::SpawnDemoEnemies(APawn* PlayerPawn)
{
	if (bSpawnedDemoEnemies || !PlayerPawn)
	{
		return;
	}
	bSpawnedDemoEnemies = true;

	UWorld* World = GetWorld();
	for (TActorIterator<AEnemyTarget> Iterator(World); Iterator; ++Iterator)
	{
		return;
	}

	const FVector Forward = PlayerPawn->GetActorForwardVector();
	const FVector Right = PlayerPawn->GetActorRightVector();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int32 Index = 0; Index < 2; ++Index)
	{
		const FVector Location = PlayerPawn->GetActorLocation()
			+ Forward * (500.0f + Index * 250.0f)
			+ Right * (Index == 0 ? -150.0f : 150.0f);
		World->SpawnActor<AEnemyTarget>(Location, FRotator::ZeroRotator, SpawnParams);
	}
}

void UGameplaySubsystem::ReturnControlToCharacter(APlayerController* Controller, ABuggyPawn* Buggy)
{
	UWorld* World = GetWorld();
	APawn* CharacterPawn = nullptr;
	for (TActorIterator<ACharacter> Iterator(World); Iterator; ++Iterator)
	{
		APawn* Candidate = *Iterator;
		if (Candidate != Buggy && !Candidate->IsA<ABumperCarPawn>() && !Candidate->IsA<AEnemyTarget>())
		{
			CharacterPawn = Candidate;
			break;
		}
	}

	if (!CharacterPawn)
	{
		UClass* PawnClass = nullptr;
		if (AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			if (GameMode->DefaultPawnClass && !GameMode->DefaultPawnClass->IsChildOf(ABumperCarPawn::StaticClass()))
			{
				PawnClass = GameMode->DefaultPawnClass;
			}
		}
		if (!PawnClass)
		{
			PawnClass = LoadClass<APawn>(nullptr, TEXT("/Game/Blueprints/SandboxCharacter_Mover.SandboxCharacter_Mover_C"));
		}
		if (PawnClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			const FVector Location = Buggy->GetActorLocation() + Buggy->GetActorRightVector() * 250.0f + FVector(0.0f, 0.0f, 100.0f);
			CharacterPawn = World->SpawnActor<APawn>(PawnClass, Location, FRotator::ZeroRotator, SpawnParams);
		}
	}

	if (CharacterPawn)
	{
		Controller->Possess(CharacterPawn);
	}
}
