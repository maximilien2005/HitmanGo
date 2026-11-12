
#include "MainCellComponent.h"

void UMainCellComponent::ExecuteCaseLogic_Implementation(AActor* InteractingActor, FIntVector2 CellCoordinates) {
	if (!ensure(IsValid(InteractingActor))) {
		return;
	}
}
