#include "PC_PlagueTaleGo.h"
#include "Kismet/GameplayStatics.h"
#include "Grid/GridManager.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GS_PlagueTaleGo.h"
#include "Pawns/MasterPawn.h"

void  APC_PlagueTaleGo::BeginPlay() {
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (!ensure(IsValid(PlagueGoInputMapping))) {
			return;
		}
		Subsystem->AddMappingContext(PlagueGoInputMapping, 0);
	}

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (!ensure(IsValid(GridManager))) {
		return;
	}
}

void APC_PlagueTaleGo::SetupInputComponent() {
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	if (!ensure(IsValid(ClickAction))) {
		return;
	}
	EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &APC_PlagueTaleGo::OnClickStarted);
}

void APC_PlagueTaleGo::OnClickStarted() {
	UE_LOG(LogTemp, Warning, TEXT("Clic started !"));

	AGS_PlagueTaleGo* GS = GetWorld()->GetGameState<AGS_PlagueTaleGo>();
	if (!ensure(IsValid(GS)) || !ensure(IsValid(GridManager))) {
		return;
	}

	if (GS->CurrentGamePhase == EGamePhase::PlayerTurn) {
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		if (HitResult.GetActor() == GridManager->ActivePlayerPawn) {
			bIsPawnSelected = true;
			GridManager->ShowCellHighlights(GridManager->GetConnectedCells(GridManager->ActivePlayerPawn->CurrentGridPosition, GridManager->ActivePlayerPawn));
			return;
		}
		else {
			ClearPawnSelection();
			return;
		}
		
		if (!bIsPawnSelected) {
			ClearPawnSelection();
			return;
		}
	}
}

void APC_PlagueTaleGo::ClearPawnSelection() {
	bIsPawnSelected = false;
	GridManager->ClearCellHighlights();
}