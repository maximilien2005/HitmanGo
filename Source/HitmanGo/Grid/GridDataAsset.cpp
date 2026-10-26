#include "GridDataAsset.h"
#include "GridManager.h"
#include "UObject/UObjectIterator.h"

FVector2D UGridDataAsset::GetPathCoordsKey(FIntVector2 GridCellPositionA, FIntVector2 GridCellPositionB) const {
	return FVector2D(
		(GridCellPositionA.X + GridCellPositionB.X) / 2.0f,
		(GridCellPositionA.Y + GridCellPositionB.Y) / 2.0f
	);
}

#if WITH_EDITOR
void UGridDataAsset::GenerateEditorPath() {
	FIntVector2 CurrentCell = PathStartCell;
	while (CurrentCell != PathEndCell) {
		FIntVector2 NextCell = CurrentCell;
		if (CurrentCell.X < PathEndCell.X) NextCell.X++;
		else if (CurrentCell.X > PathEndCell.X) NextCell.X--;
		else if (CurrentCell.Y < PathEndCell.Y) NextCell.Y++;
		else if (CurrentCell.Y > PathEndCell.Y) NextCell.Y--;

		FChemainStruct NewPath;
		NewPath.GridCellPositionA = CurrentCell;

		NewPath.GridCellPositionB = NextCell;
		NewPath.SpecialPathClass = nullptr;

		FVector2D PathKey = GetPathCoordsKey(CurrentCell, NextCell);
		listChemainData.Add(PathKey, NewPath);

		CurrentCell = NextCell;
	}

	MarkPackageDirty();
	RefreshConnectedGridManagers();
}

void UGridDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Recalcule automatiquement les clés de la Map si on modifie les coordonnées A ou B à la main dans le Data Asset
	TMap<FVector2D, FChemainStruct> CorrectedPathMap;
	for (const TPair<FVector2D, FChemainStruct>& ChainDataPair : listChemainData) {
		FVector2D CorrectKey = GetPathCoordsKey(ChainDataPair.Value.GridCellPositionA, ChainDataPair.Value.GridCellPositionB);
		CorrectedPathMap.Add(CorrectKey, ChainDataPair.Value);
	}
	listChemainData = CorrectedPathMap;

	RefreshConnectedGridManagers();
}

void UGridDataAsset::RefreshConnectedGridManagers() {
	// Met à jour automatiquement tous les GridManagers dans l'éditeur qui utilisent ce Data Asset
	for (TObjectIterator<AGridManager> It; It; ++It) {
		AGridManager* Grid = *It;
		if (IsValid(Grid) && !Grid->IsTemplate() && Grid->GridData == this) {
			Grid->RerunConstructionScripts();
		}
	}
}
#endif