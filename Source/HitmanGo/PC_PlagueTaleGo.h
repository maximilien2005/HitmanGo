// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_PlagueTaleGo.generated.h"

class UInputMappingContext;
class UInputAction;
class AGridManager;

UCLASS()
class HITMANGO_API APC_PlagueTaleGo : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input|Mapping")
	UInputMappingContext* PlagueGoInputMapping;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
	UInputAction* ClickAction;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void OnClickStarted();

	void ClearPawnSelection();

	UPROPERTY(Transient)
	AGridManager* GridManager;

	bool bIsPawnSelected = false;
};
