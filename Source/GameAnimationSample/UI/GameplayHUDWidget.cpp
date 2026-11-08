#include "GameAnimationSample/UI/GameplayHUDWidget.h"

#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameAnimationSample/Character/HealthComponent.h"
#include "GameAnimationSample/Weapons/WeaponManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Text/STextBlock.h"

TSharedRef<SWidget> UGameplayHUDWidget::RebuildWidget()
{
	PlayerBar = SNew(SProgressBar).Percent(1.0f).FillColorAndOpacity(FLinearColor(0.2f, 0.85f, 0.25f));
	EnemyBar = SNew(SProgressBar).Percent(1.0f).FillColorAndOpacity(FLinearColor(0.85f, 0.2f, 0.2f));

	PlayerText = SNew(STextBlock).Justification(ETextJustify::Center).ColorAndOpacity(FLinearColor::White);
	EnemyText = SNew(STextBlock).Justification(ETextJustify::Center).ColorAndOpacity(FLinearColor::White);

	return SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(0.0f, 0.0f, 0.0f, 68.0f))
		[
			SAssignNew(WeaponText, STextBlock)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FLinearColor(0.85f, 0.85f, 0.85f))
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(0.0f, 0.0f, 0.0f, 40.0f))
		[
			SNew(SBox)
			.WidthOverride(400.0f)
			.HeightOverride(22.0f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()[PlayerBar.ToSharedRef()]
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[PlayerText.ToSharedRef()]
			]
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0.0f, 30.0f, 0.0f, 0.0f))
		[
			SAssignNew(EnemyContainer, SBox)
			.WidthOverride(500.0f)
			.HeightOverride(20.0f)
			.Visibility(EVisibility::Collapsed)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()[EnemyBar.ToSharedRef()]
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[EnemyText.ToSharedRef()]
			]
		];
}

void UGameplayHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePlayerHealth();
	UWorld* World = GetWorld();
	UpdateEnemyHealth(World ? World->GetTimeSeconds() : 0.0f);
}

void UGameplayHUDWidget::UpdatePlayerHealth()
{
	APawn* Pawn = GetOwningPlayerPawn();
	if (Pawn)
	{
		if (UWeaponManagerComponent* Weapons = Pawn->FindComponentByClass<UWeaponManagerComponent>())
		{
			const TCHAR* WeaponName = TEXT("徒手");
			switch (Weapons->GetCurrentWeaponType())
			{
			case EWeaponType::Katana:
				WeaponName = TEXT("太刀");
				break;
			case EWeaponType::TangDao:
				WeaponName = TEXT("唐刀");
				break;
			case EWeaponType::AK47:
				WeaponName = TEXT("AK47");
				break;
			default:
				break;
			}
			WeaponText->SetText(FText::FromString(FString::Printf(TEXT("%s   [Tab 切换 / 左键攻击]"), WeaponName)));
		}
		else
		{
			WeaponText->SetText(FText::GetEmpty());
		}
	}
	UHealthComponent* Health = Pawn ? Pawn->FindComponentByClass<UHealthComponent>() : nullptr;
	if (!Health)
	{
		PlayerBar->SetPercent(0.0f);
		PlayerText->SetText(FText::GetEmpty());
		return;
	}

	const float Percent = Health->GetHealthPercent();
	PlayerBar->SetPercent(Percent);
	PlayerBar->SetFillColorAndOpacity(Percent > 0.5f
		? FLinearColor(0.2f, 0.85f, 0.25f)
		: (Percent > 0.25f ? FLinearColor(0.95f, 0.75f, 0.15f) : FLinearColor(0.9f, 0.15f, 0.15f)));
	PlayerText->SetText(FText::FromString(FString::Printf(TEXT("HP %d / %d"),
		FMath::CeilToInt(Health->CurrentHealth), FMath::CeilToInt(Health->MaxHealth))));
}

AActor* UGameplayHUDWidget::FindAimedEnemy() const
{
	APlayerController* Controller = GetOwningPlayer();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	if (!Controller || !Controller->PlayerCameraManager || !Pawn)
	{
		return nullptr;
	}

	const FVector Start = Controller->PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + Controller->PlayerCameraManager->GetCameraRotation().Vector() * 5000.0f;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(HudAim), false, Pawn);
	TArray<FHitResult> Hits;
	GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_Pawn, QueryParams);

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == Pawn)
		{
			continue;
		}
		UHealthComponent* Health = HitActor->FindComponentByClass<UHealthComponent>();
		if (Health && !Health->IsDead())
		{
			return HitActor;
		}
		if (Hit.bBlockingHit)
		{
			break;
		}
	}
	return nullptr;
}

void UGameplayHUDWidget::UpdateEnemyHealth(float CurrentTime)
{
	if (AActor* AimedEnemy = FindAimedEnemy())
	{
		TrackedEnemy = AimedEnemy;
		EnemyHideAtTime = CurrentTime + 3.0f;
	}

	AActor* Enemy = TrackedEnemy.Get();
	UHealthComponent* Health = Enemy ? Enemy->FindComponentByClass<UHealthComponent>() : nullptr;
	if (!Health || Health->IsDead() || CurrentTime > EnemyHideAtTime)
	{
		TrackedEnemy.Reset();
		EnemyContainer->SetVisibility(EVisibility::Collapsed);
		return;
	}

	EnemyContainer->SetVisibility(EVisibility::Visible);
	EnemyBar->SetPercent(Health->GetHealthPercent());
	EnemyText->SetText(FText::FromString(FString::Printf(TEXT("%s  %d / %d"),
		*Enemy->GetName(), FMath::CeilToInt(Health->CurrentHealth), FMath::CeilToInt(Health->MaxHealth))));
}
