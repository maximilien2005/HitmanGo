#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "MainCaseComponent.generated.h"

class AGridManager;
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HITMANGO_API UMainCaseComponent : public UStaticMeshComponent {
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	AGridManager* GridManagerRef;

	// Fonction appelée quand un acteur interagit avec cette case spéciale
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid Interaction")
	void ExecuteCaseLogic(AActor* InteractingActor, FIntVector2 CellCoordinates);

	virtual void ExecuteCaseLogic_Implementation(AActor* InteractingActor, FIntVector2 CellCoordinates);
};