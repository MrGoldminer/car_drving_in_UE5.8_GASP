#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrapplingHookComponent.generated.h"

class USplineMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UGrapplingHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrapplingHookComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void StartGrapple();

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void ReleaseGrapple();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grappling Hook")
	bool bIsGrappling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grappling Hook")
	FVector HookPoint = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grappling Hook", meta = (ClampMin = "100.0", Units = "cm"))
	float MaxDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grappling Hook", meta = (ClampMin = "100.0", Units = "cm"))
	float StopDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grappling Hook", meta = (ClampMin = "0.0", Units = "cm/s"))
	float PullSpeed = 2400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grappling Hook", meta = (ClampMin = "0.1"))
	float PullResponsiveness = 6.0f;

private:
	UPROPERTY()
	TObjectPtr<USplineMeshComponent> RopeMesh;

	TWeakObjectPtr<UPrimitiveComponent> HookComponent;
	FVector HookLocalPoint = FVector::ZeroVector;
	TWeakObjectPtr<UInputComponent> BoundInputComponent;

	void UpdateRope();
};
