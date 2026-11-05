// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_PlagueTaleGo.generated.h"

class AMasterPawn;

UCLASS()
class HITMANGO_API AGM_PlagueTaleGo : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void PossessPlayerPawn(AMasterPawn* PlayerPawn);
};
