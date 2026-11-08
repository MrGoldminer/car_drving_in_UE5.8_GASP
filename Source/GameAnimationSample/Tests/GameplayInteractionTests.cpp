#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameAnimationSample/Character/VehicleInteractionComponent.h"
#include "GameAnimationSample/Tests/TestDamageReceiver.h"
#include "GameAnimationSample/Vehicles/BuggyPawn.h"
#include "GameAnimationSample/Weapons/MeleeWeapon.h"
#include "GameAnimationSample/Weapons/RifleWeapon.h"
#include "GameAnimationSample/Weapons/WeaponManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

namespace GameplayTestHelpers
{
	UWorld* CreateTestWorld()
	{
		UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
		if (World)
		{
			World->InitializeActorsForPlay(FURL());
			World->BeginPlay();
		}
		return World;
	}

	void DestroyTestWorld(UWorld* World)
	{
		if (World)
		{
			GEngine->DestroyWorldContext(World);
			World->DestroyWorld(false);
		}
	}

	ACharacter* SpawnControlledCharacter(UWorld* World, APlayerController*& OutController)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ACharacter* Character = World->SpawnActor<ACharacter>(FVector(0.0f, 0.0f, 100.0f), FRotator::ZeroRotator, Params);
		Character->DispatchBeginPlay();
		OutController = World->SpawnActor<APlayerController>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
		OutController->Possess(Character);
		return Character;
	}

	ABuggyPawn* SpawnBuggy(UWorld* World, const FVector& Location)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		return World->SpawnActor<ABuggyPawn>(Location, FRotator::ZeroRotator, Params);
	}

	ATestDamageReceiver* SpawnTarget(UWorld* World, const FVector& Location)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATestDamageReceiver* Target = World->SpawnActor<ATestDamageReceiver>(Location, FRotator::ZeroRotator, Params);
		Target->DispatchBeginPlay();
		return Target;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVehicleEnterExitTest,
	"GameAnimationSample.Vehicles.EnterExitRestoresCharacter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVehicleEnterExitTest::RunTest(const FString& Parameters)
{
	UWorld* World = GameplayTestHelpers::CreateTestWorld();
	if (!World)
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	APlayerController* Controller = nullptr;
	ACharacter* Character = GameplayTestHelpers::SpawnControlledCharacter(World, Controller);
	ABuggyPawn* Buggy = GameplayTestHelpers::SpawnBuggy(World, FVector(150.0f, 0.0f, 50.0f));

	TestTrue(TEXT("EnterVehicle succeeds"), Buggy->EnterVehicle(Character));
	TestTrue(TEXT("Buggy has driver"), Buggy->HasDriver());
	TestEqual(TEXT("Controller possesses buggy"), static_cast<APawn*>(Controller->GetPawn()), static_cast<APawn*>(Buggy));
	TestFalse(TEXT("Character tick disabled"), Character->IsActorTickEnabled());
	TestFalse(TEXT("Character collision disabled"), Character->GetActorEnableCollision());

	TestTrue(TEXT("ExitVehicle succeeds"), Buggy->ExitVehicle());
	TestFalse(TEXT("Buggy driver cleared"), Buggy->HasDriver());
	TestEqual(TEXT("Controller back on character"), static_cast<APawn*>(Controller->GetPawn()), static_cast<APawn*>(Character));
	TestTrue(TEXT("Character tick restored"), Character->IsActorTickEnabled());
	TestTrue(TEXT("Character collision restored"), Character->GetActorEnableCollision());
	TestFalse(TEXT("Character visible again"), Character->IsHidden());

	GameplayTestHelpers::DestroyTestWorld(World);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVehicleProximityTest,
	"GameAnimationSample.Vehicles.FSelectsNearestEmptyBuggy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVehicleProximityTest::RunTest(const FString& Parameters)
{
	UWorld* World = GameplayTestHelpers::CreateTestWorld();
	if (!World)
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	APlayerController* Controller = nullptr;
	ACharacter* Character = GameplayTestHelpers::SpawnControlledCharacter(World, Controller);
	ABuggyPawn* FarBuggy = GameplayTestHelpers::SpawnBuggy(World, FVector(280.0f, 0.0f, 50.0f));
	ABuggyPawn* NearBuggy = GameplayTestHelpers::SpawnBuggy(World, FVector(120.0f, 0.0f, 50.0f));
	ABuggyPawn* OutOfRangeBuggy = GameplayTestHelpers::SpawnBuggy(World, FVector(900.0f, 0.0f, 50.0f));

	UVehicleInteractionComponent* Interaction = NewObject<UVehicleInteractionComponent>(Character, TEXT("TestInteraction"));
	Interaction->RegisterComponent();
	Interaction->TryEnterVehicle();

	TestTrue(TEXT("Nearest buggy occupied"), NearBuggy->HasDriver());
	TestFalse(TEXT("Far buggy untouched"), FarBuggy->HasDriver());
	TestFalse(TEXT("Out of range buggy untouched"), OutOfRangeBuggy->HasDriver());
	TestEqual(TEXT("Controller on nearest buggy"), static_cast<APawn*>(Controller->GetPawn()), static_cast<APawn*>(NearBuggy));

	GameplayTestHelpers::DestroyTestWorld(World);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWeaponCycleTest,
	"GameAnimationSample.Weapons.TabCyclesAllWeapons",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponCycleTest::RunTest(const FString& Parameters)
{
	UWorld* World = GameplayTestHelpers::CreateTestWorld();
	if (!World)
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	APlayerController* Controller = nullptr;
	ACharacter* Character = GameplayTestHelpers::SpawnControlledCharacter(World, Controller);

	UWeaponManagerComponent* Manager = NewObject<UWeaponManagerComponent>(Character, TEXT("TestWeaponManager"));
	Manager->RegisterComponent();

	TestTrue(TEXT("Character has begun play"), Character->HasActorBegunPlay());
	TestTrue(TEXT("Manager has begun play"), Manager->HasBegunPlay());
	FActorSpawnParameters DirectParams;
	AMeleeWeapon* DirectWeapon = World->SpawnActor<AMeleeWeapon>(DirectParams);
	TestNotNull(TEXT("Direct weapon spawn works"), DirectWeapon);

	TestEqual(TEXT("Starts unarmed"), Manager->GetCurrentWeaponType(), EWeaponType::None);

	const EWeaponType ExpectedOrder[] = {
		EWeaponType::Katana,
		EWeaponType::TangDao,
		EWeaponType::AK47,
		EWeaponType::None,
	};
	for (EWeaponType Expected : ExpectedOrder)
	{
		Manager->CycleWeapon();
		TestEqual(TEXT("Cycle advances"), Manager->GetCurrentWeaponType(), Expected);
		if (Expected != EWeaponType::None)
		{
			AWeaponBase* Weapon = Manager->GetCurrentWeapon();
			TestNotNull(TEXT("Weapon exists"), Weapon);
			if (Weapon)
			{
				TestFalse(TEXT("Equipped weapon visible"), Weapon->IsHidden());
				TestEqual(TEXT("Weapon attached to character"), Weapon->GetAttachParentActor(), static_cast<AActor*>(Character));
			}
		}
	}

	GameplayTestHelpers::DestroyTestWorld(World);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRifleDamageTest,
	"GameAnimationSample.Weapons.AK47ShotDamagesTarget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRifleDamageTest::RunTest(const FString& Parameters)
{
	UWorld* World = GameplayTestHelpers::CreateTestWorld();
	if (!World)
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	APlayerController* Controller = nullptr;
	ACharacter* Character = GameplayTestHelpers::SpawnControlledCharacter(World, Controller);
	Controller->PlayerCameraManager = nullptr;
	ATestDamageReceiver* Target = GameplayTestHelpers::SpawnTarget(World, FVector(500.0f, 0.0f, 100.0f));

	UWeaponManagerComponent* Manager = NewObject<UWeaponManagerComponent>(Character, TEXT("TestWeaponManager"));
	Manager->RegisterComponent();
	Manager->EquipWeapon(EWeaponType::AK47);

	ARifleWeapon* Rifle = Cast<ARifleWeapon>(Manager->GetCurrentWeapon());
	TestNotNull(TEXT("Rifle equipped"), Rifle);
	if (!Rifle)
	{
		GameplayTestHelpers::DestroyTestWorld(World);
		return false;
	}

	Rifle->StartAttack();
	Rifle->Tick(0.05f);

	TestTrue(TEXT("Target took rifle damage"), Target->AccumulatedDamage > 0.0f);

	GameplayTestHelpers::DestroyTestWorld(World);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMeleeDamageTest,
	"GameAnimationSample.Weapons.KatanaSwingDamagesTargetOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMeleeDamageTest::RunTest(const FString& Parameters)
{
	UWorld* World = GameplayTestHelpers::CreateTestWorld();
	if (!World)
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	APlayerController* Controller = nullptr;
	ACharacter* Character = GameplayTestHelpers::SpawnControlledCharacter(World, Controller);
	ATestDamageReceiver* Target = GameplayTestHelpers::SpawnTarget(World, FVector(150.0f, 0.0f, 100.0f));

	UWeaponManagerComponent* Manager = NewObject<UWeaponManagerComponent>(Character, TEXT("TestWeaponManager"));
	Manager->RegisterComponent();
	Manager->EquipWeapon(EWeaponType::Katana);

	AMeleeWeapon* Katana = Cast<AMeleeWeapon>(Manager->GetCurrentWeapon());
	TestNotNull(TEXT("Katana equipped"), Katana);
	if (!Katana)
	{
		GameplayTestHelpers::DestroyTestWorld(World);
		return false;
	}

	Katana->StartAttack();
	for (int32 Step = 0; Step < 10; ++Step)
	{
		Katana->Tick(0.05f);
	}

	TestTrue(TEXT("Target took melee damage"), Target->AccumulatedDamage > 0.0f);
	TestEqual(TEXT("Single swing hits once"), Target->AccumulatedDamage, Katana->Damage);

	GameplayTestHelpers::DestroyTestWorld(World);
	return true;
}

#endif
