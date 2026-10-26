
#include "MainCaseComponent.h"

void UMainCaseComponent::ExecuteCaseLogic_Implementation(AActor* InteractingActor, FIntVector2 CellCoordinates) {
	if (!ensure(IsValid(InteractingActor))) {
		return;
	}
}
