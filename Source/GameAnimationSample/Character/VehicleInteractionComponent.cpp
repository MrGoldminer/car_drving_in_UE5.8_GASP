#include "GameAnimationSample/Character/VehicleInteractionComponent.h"

#include "Components/InputComponent.h"
#include "GameAnimationSample/Vehicles/BuggyPawn.h"
#include "GameFramework/PlayerController.h"

UVehicleInteractionComponent::UVehicleInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleInteractionComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!bInputBound && OwnerPawn && OwnerPawn->InputComponent)
	{
		OwnerPawn->InputComponent->BindAction(
			TEXT("VehicleInteract"),
			IE_Pressed,
			this,
			&UVehicleInteractionComponent::TryEnterVehicle);
		bInputBound = true;
	}
}

void UVehicleInteractionComponent::TryEnterVehicle()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* Controller = Cast<APlayerController>(OwnerPawn->GetController());
	if (!Controller || !Controller->PlayerCameraManager)
	{
		return;
	}

	const FVector TraceStart = Controller->PlayerCameraManager->GetCameraLocation();
	const FVector TraceEnd = TraceStart + Controller->PlayerCameraManager->GetCameraRotation().Vector() * InteractionDistance;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VehicleInteraction), false, OwnerPawn);
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		return;
	}

	if (ABuggyPawn* Vehicle = Cast<ABuggyPawn>(Hit.GetActor()))
	{
		Vehicle->EnterVehicle(OwnerPawn);
	}
}
