// Fill out your copyright notice in the Description page of Project Settings.

#include "GM_PlagueTaleGo.h"
#include "Pawns/MasterPawn.h"
#include "GameFramework/PlayerController.h"

void AGM_PlagueTaleGo::PossessPlayerPawn(AMasterPawn* PlayerPawn) {
	if (!ensure(IsValid(PlayerPawn))) {
		return;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!ensure(IsValid(PlayerController))) {
		return;
	}

	PlayerController->Possess(PlayerPawn);
}