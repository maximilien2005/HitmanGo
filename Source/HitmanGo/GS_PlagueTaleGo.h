// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_PlagueTaleGo.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8 {
	PlayerTurn  UMETA(DisplayName = "Player Turn"),
	HugoTurn	UMETA(DisplayName = "Hugo Turn"),
	EnemiesTurn UMETA(DisplayName = "Enemies Turn"),
	GameOver	UMETA(DisplayName = "Game Over")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);

UCLASS()
class HITMANGO_API AGS_PlagueTaleGo : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Category = "GamePhase")
	bool SetNewGamePhase(EGamePhase NewPhase);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePhase")
	EGamePhase CurrentGamePhase;
private:

	UPROPERTY(BlueprintAssignable, Category = "Event|GamePhase")
	FOnGamePhaseChanged OnGamePhaseChanged;
};
