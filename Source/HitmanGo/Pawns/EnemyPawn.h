// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/MasterPawn.h"
#include "EnemyPawn.generated.h"

/**
 * 
 */
UCLASS()
class HITMANGO_API AEnemyPawn : public AMasterPawn
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void ExecutePawnTurn() override;
};
