#include "GameAnimationSample/Character/VehicleInteractionComponent.h"

#include "Components/InputComponent.h"
#include "EngineUtils.h"
#include "GameAnimationSample/Vehicles/BuggyPawn.h"

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
	if (OwnerPawn && OwnerPawn->InputComponent && BoundInputComponent.Get() != OwnerPawn->InputComponent)
	{
		OwnerPawn->InputComponent->BindAction(
			TEXT("VehicleInteract"),
			IE_Pressed,
			this,
			&UVehicleInteractionComponent::TryEnterVehicle);
		BoundInputComponent = OwnerPawn->InputComponent;
	}
}

void UVehicleInteractionComponent::TryEnterVehicle()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	ABuggyPawn* ClosestVehicle = nullptr;
	float ClosestDistanceSq = FMath::Square(InteractionDistance);
	for (TActorIterator<ABuggyPawn> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		ABuggyPawn* Vehicle = *Iterator;
		if (Vehicle->HasDriver())
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(OwnerPawn->GetActorLocation(), Vehicle->GetActorLocation());
		if (DistanceSq <= ClosestDistanceSq)
		{
			ClosestDistanceSq = DistanceSq;
			ClosestVehicle = Vehicle;
		}
	}

	if (ClosestVehicle)
	{
		ClosestVehicle->EnterVehicle(OwnerPawn);
	}
}
