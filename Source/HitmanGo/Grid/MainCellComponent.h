#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "MainCellComponent.generated.h"

class AMasterPawn;
class AGridManager;
struct FSpecialCellConfig;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HITMANGO_API UMainCellComponent : public UStaticMeshComponent {
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	AGridManager* GridManager;
	virtual void ExecuteCellLogic(AMasterPawn* InteractingPawn);

	virtual void InitializeCell(AGridManager* Grid, FIntVector2 Coords, const FSpecialCellConfig& Config);

	virtual bool CanBeActivatedBy(const AMasterPawn* Pawn) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	FIntVector2 CellCoordinates;
};