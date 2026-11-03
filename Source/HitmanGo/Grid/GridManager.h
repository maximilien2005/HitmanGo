#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridDataAsset.h"
#include "GridManager.generated.h"

class AMasterPawn;
class UChildActorComponent;

UCLASS()
class HITMANGO_API AGridManager : public AActor {
	GENERATED_BODY()

public:
	AGridManager();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Board;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Cells;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Paths;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* CellInstancedMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* PathInstancedMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* HighlightInstancedMesh;

private:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void BuildUsedCellInstances();
	void BuildSpecialCases();
	void BuildPaths();
	AMasterPawn* SpawnPawnOnGrid(TSubclassOf<AMasterPawn> PawnClass, FIntVector2 CellCoords, EGridDirection Direction);
	void SpawnPlayer();
	void SpawnEnemies();

	void ClearGrid();


#if WITH_EDITOR
	UChildActorComponent* CreatePawnPreview(USceneComponent* ParentGroup, TSubclassOf<AMasterPawn> PawnClass, FIntVector2 CellCoords, EGridDirection Direction);
	void BuildPlayerPreview();
	void BuildEnemiesPreview();
	void BuildCellInstances();
	void BuildDebugTexts();


	void ClearEditorVisuals();
	void ClearPreviewGroup(USceneComponent* ParentGroup);
	void ClearDebugTexts();
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editable | MapParameters")
	UGridDataAsset* GridData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editable | MapParameters")
	float EspaceCells = 200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapParameters | Runtime")
	TMap<FIntVector2, UMainCaseComponent*> RuntimeSpecialCases;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapParameters | Runtime")
	TMap<FVector2D, UMainPathComponent*> RuntimeSpecialPaths;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "MapParameters | Runtime")
	AMasterPawn* ActivePlayerPawn = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "MapParameters | Runtime")
	TArray<AMasterPawn*> ActiveEnemies;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	TArray<FIntVector2> GetConnectedCells(FIntVector2 PositionCheck, AActor* MovingActor = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool CanMoveBetweenCells(FIntVector2 StartCell, FIntVector2 TargetCell, AActor* MovingActor = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "Grid | Direction")
	bool CanMoveInDirection(FIntVector2 StartCell, EGridDirection Direction, AActor* MovingActor, FIntVector2& OutTargetCell) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid | Direction")
	bool GetDirectionBetweenCells(FIntVector2 StartCell, FIntVector2 TargetCell, EGridDirection& OutDirection) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid | Direction")
	static EGridDirection GetOppositeDirection(EGridDirection Direction);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid | Direction")
	static FIntVector2 DirectionToOffset(EGridDirection Direction);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid | Direction")
	static FRotator DirectionToLocalRotation(EGridDirection Direction);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid | Direction")
	FRotator DirectionToWorldRotation(EGridDirection Direction) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid | Direction")
	EGridDirection WorldRotationToDirection(FRotator WorldRotation) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	UMainPathComponent* GetSpecialPathBetween(FIntVector2 CellA, FIntVector2 CellB) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	FVector2D GetPathCoordsKey(FIntVector2 GridCellPositionA, FIntVector2 GridCellPositionB) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	FVector GridToWorldLocation(FIntVector2 CellCoords) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	FVector GridToLocalLocation(FIntVector2 CellCoords) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	bool WorldToGridLocation(FVector WorldLocation, FIntVector2& OutCellCoords, float MaxDistanceTolerance = 75.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Grid Visuals")
	void ShowCellHighlights(const TArray<FIntVector2>& CellsToHighlight);

	UFUNCTION(BlueprintCallable, Category = "Grid Visuals")
	void ClearCellHighlights();

	UFUNCTION(BlueprintCallable, Category = "Grid Interaction")
	void TriggerCell(FIntVector2 CellCoords, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Grid Interaction")
	void TriggerPath(FVector2D PathCoords, AActor* TargetActor);

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Previews;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* PlayersPreview;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* EnemiesPreview;

	UPROPERTY(EditAnywhere, Category = "Editable | Debug")
	bool bLookCoords = false;
#endif
};