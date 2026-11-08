#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayHUDWidget.generated.h"

class SProgressBar;
class STextBlock;

UCLASS()
class GAMEANIMATIONSAMPLE_API UGameplayHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TSharedPtr<SProgressBar> PlayerBar;
	TSharedPtr<STextBlock> PlayerText;
	TSharedPtr<SBox> EnemyContainer;
	TSharedPtr<SProgressBar> EnemyBar;
	TSharedPtr<STextBlock> EnemyText;

	TWeakObjectPtr<AActor> TrackedEnemy;
	float EnemyHideAtTime = 0.0f;

	void UpdatePlayerHealth();
	void UpdateEnemyHealth(float CurrentTime);
	AActor* FindAimedEnemy() const;
};
