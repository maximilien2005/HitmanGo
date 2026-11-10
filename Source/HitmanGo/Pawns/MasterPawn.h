#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Grid/GridDataAsset.h"
#include "MasterPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class AGridManager;

UCLASS(Abstract)
class HITMANGO_API AMasterPawn : public APawn {
	GENERATED_BODY()

public:
	AMasterPawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PawnStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* SelectionCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseTop;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid")
	FIntVector2 CurrentGridPosition = FIntVector2(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid | Direction")
	EGridDirection CurrentDirection = EGridDirection::North;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	AGridManager* GridManagerRef = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual void InitializePawnOnGrid(FIntVector2 StartPosition, EGridDirection StartDirection, AGridManager* Manager);

	UFUNCTION(BlueprintCallable, Category = "Grid | Turn")
	virtual void ExecutePawnTurn();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	bool CanMoveToCell(FIntVector2 TargetCellCoords) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid | Direction")
	bool CanMoveInDirection(EGridDirection Direction, FIntVector2& OutTargetCell) const;

	UFUNCTION(BlueprintCallable, Category = "Grid | Direction")
	void SetFacingDirection(EGridDirection NewDirection);

	UFUNCTION(BlueprintCallable, Category = "Grid | Direction")
	void TurnAround();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual bool MoveToCell(FIntVector2 TargetCellCoords);

	UFUNCTION(BlueprintCallable, Category = "Grid | Direction")
	virtual bool MoveInDirection(EGridDirection Direction, bool bTurnAroundAtDeadEnd = true);

	UFUNCTION(BlueprintCallable, Category = "Grid | Direction")
	virtual bool StepForwardPatrol();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual void TeleportToCell(FIntVector2 TargetCellCoords, bool bTriggerDestinationCell = false);
};