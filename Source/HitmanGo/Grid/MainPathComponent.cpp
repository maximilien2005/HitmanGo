#include "MainPathComponent.h"
#include "Pawns/MasterPawn.h"

void UMainPathComponent::ExecutePathLogic_Implementation(AActor* InteractingActor, FVector2D InPathCoordinates)
{
	if (!ensure(IsValid(InteractingActor))) {
		return;
	}
}
bool UMainPathComponent::IsPraticable_Implementation(AActor* InteractingActor, FIntVector2 ActorCellCoordinates) {
	return true;
}