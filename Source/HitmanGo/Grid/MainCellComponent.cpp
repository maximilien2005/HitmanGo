#include "MainCellComponent.h"
#include "GridDataAsset.h"

void UMainCellComponent::ExecuteCellLogic(AMasterPawn* InteractingPawn) {
}

void UMainCellComponent::InitializeCell(AGridManager* Grid, FIntVector2 Coords, const FSpecialCellConfig& Config) {
	GridManager = Grid;
	CellCoordinates = Coords;
}

bool UMainCellComponent::CanBeActivatedBy(const AMasterPawn* Pawn) const {
	return true;
}