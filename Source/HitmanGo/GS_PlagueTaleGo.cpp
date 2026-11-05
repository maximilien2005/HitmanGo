// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_PlagueTaleGo.h"

bool AGS_PlagueTaleGo::SetNewGamePhase(EGamePhase NewPhase)
{
	if (CurrentGamePhase != NewPhase)
	{
		CurrentGamePhase = NewPhase;
		OnGamePhaseChanged.Broadcast(NewPhase);
		return true;
	}
	return false;
}