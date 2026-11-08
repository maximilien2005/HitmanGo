// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_PlagueTaleGo.generated.h"

class AMasterPawn;
class AGridManager;
class AGS_PlagueTaleGo;

UCLASS()
class HITMANGO_API AGM_PlagueTaleGo : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void PossessPlayerPawn(AMasterPawn* PlayerPawn);

	UFUNCTION(BlueprintCallable, Category = "Turn Management")
	void OnPlayerMoveFinished();

	UFUNCTION(BlueprintCallable, Category = "Turn Management")
	void OnExitReached(AActor* Actor);

protected:
	virtual void BeginPlay() override;

private:
	virtual void InitGameState() override;
	void GameOver();

	void RunEnemiesTurn();
	void OnEnemiesMoveFinished();

	void RunHugoTurn();
	void OnHugoMoveFinished();

	void HandleDefeat(AMasterPawn* DefeatedPawn);
	bool IsPawnCaughtBy(AMasterPawn* FriendlyPawn, AMasterPawn* EnemyPawn) const;
	bool IsPawnOnEnemyCell(AMasterPawn* FriendlyPawn) const;

	UPROPERTY(Transient)
	AGridManager* GridManager;

	UPROPERTY(Transient)
	AGS_PlagueTaleGo* PlagueGoGameState;
};
