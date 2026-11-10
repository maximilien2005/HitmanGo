// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/EnemyPawn.h"

void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyPawn::ExecutePawnTurn()
{
	MoveInDirection(CurrentDirection);
}