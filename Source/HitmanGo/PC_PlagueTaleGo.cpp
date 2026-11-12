#include "PC_PlagueTaleGo.h"
#include "Kismet/GameplayStatics.h"
#include "Grid/GridManager.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GS_PlagueTaleGo.h"
#include "Pawns/MasterPawn.h"
#include "Pawns/PlayerPawn.h"
#include "GM_PlagueTaleGo.h"

void  APC_PlagueTaleGo::BeginPlay() {
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ensure(IsValid(PlagueGoInputMapping))) {
			Subsystem->AddMappingContext(PlagueGoInputMapping, 0);
		}
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
	UE_LOG(LogTemp, Log, TEXT("OnClickStarted"));
	AGS_PlagueTaleGo* GS = GetWorld()->GetGameState<AGS_PlagueTaleGo>();
	if (!ensure(IsValid(GS)) || !ensure(IsValid(GridManager)) || !ensure(IsValid(GridManager->ActivePlayerPawn))) {
		return;
	}

	GridManager->ClearCellHighlights();

	if (GS->CurrentGamePhase == EGamePhase::PlayerTurn) {
		FHitResult HitResult;
		const bool bGotHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		if (HitResult.GetActor() == GridManager->ActivePlayerPawn) {
			bIsPawnSelected = true;
			GridManager->ShowCellHighlights(GridManager->GetConnectedCells(GridManager->ActivePlayerPawn->CurrentGridPosition, GridManager->ActivePlayerPawn));
			return;
		}

		if (bIsPawnSelected) {
			FIntVector2 TargetCellCoords;
			bool bIsValidCell = GridManager->WorldToGridLocation(HitResult.ImpactPoint, TargetCellCoords);
			if(bGotHit && bIsValidCell && GridManager->ActivePlayerPawn->MoveToCell(TargetCellCoords)) {
				AGM_PlagueTaleGo* GM = GetWorld()->GetAuthGameMode<AGM_PlagueTaleGo>();
				if (!ensure(IsValid(GM))) {
					return;
				}
				GM->OnPlayerMoveFinished();
			}
		}
	}
	bIsPawnSelected = false;
}