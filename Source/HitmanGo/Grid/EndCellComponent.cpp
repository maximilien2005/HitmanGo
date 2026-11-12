#include "Grid/EndCellComponent.h"
#include "Pawns/PlayerPawn.h"
#include "GM_PlagueTaleGo.h"

bool UEndCellComponent::CanBeActivatedBy(const AMasterPawn* Pawn) const {
	return IsValid(Pawn) && Pawn->IsA<APlayerPawn>();
}

void UEndCellComponent::ExecuteCellLogic(AMasterPawn* InteractingPawn) {
	UWorld* World = GetWorld();
	if (!ensure(IsValid(World))) {
		return;
	}

	AGM_PlagueTaleGo* GameMode = World->GetAuthGameMode<AGM_PlagueTaleGo>();
	if (!ensure(IsValid(GameMode))) {
		return;
	}

	GameMode->OnExitReached(InteractingPawn);
}
