#pragma once

#include "CoreMinimal.h"
#include "Grid/MainCellComponent.h"
#include "EndCellComponent.generated.h"

class AMasterPawn;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HITMANGO_API UEndCellComponent : public UMainCellComponent {
	GENERATED_BODY()

public:
	virtual bool CanBeActivatedBy(const AMasterPawn* Pawn) const override;
	virtual void ExecuteCellLogic(AMasterPawn* InteractingPawn) override;
};
