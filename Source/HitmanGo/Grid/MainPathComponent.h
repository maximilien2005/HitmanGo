#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "MainPathComponent.generated.h"

class AGridManager;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HITMANGO_API UMainPathComponent : public UStaticMeshComponent {
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	AGridManager* GridManagerRef = nullptr;

	// Coordonnée du chemin (ex: (1.5, 0.0))
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	FVector2D PathCoordinates = FVector2D::ZeroVector;

	// Première cellule reliée par ce chemin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	FIntVector2 GridCellPositionA = FIntVector2(0, 0);

	// Seconde cellule reliée par ce chemin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	FIntVector2 GridCellPositionB = FIntVector2(0, 0);

	// Appelé quand un acteur traverse ce chemin spécial (Clé du chemin en FVector2D, ex: (1.5, 0.0))
	UFUNCTION(BlueprintNativeEvent, Category = "Path Logic")
	void ExecutePathLogic(AActor* InteractingActor, FVector2D InPathCoordinates);
	virtual void ExecutePathLogic_Implementation(AActor* InteractingActor, FVector2D InPathCoordinates);

	// Vérifie si l'acteur situé sur la cellule ActorCellCoordinates (en FIntVector2) peut emprunter ce chemin
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid Interaction")
	bool IsPraticable(AActor* InteractingActor, FIntVector2 ActorCellCoordinates);
	virtual bool IsPraticable_Implementation(AActor* InteractingActor, FIntVector2 ActorCellCoordinates);
};