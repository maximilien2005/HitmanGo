#include "GM_PlagueTaleGo.h"
#include "Pawns/MasterPawn.h"
#include "Pawns/PlayerPawn.h"
#include "Pawns/EnemyPawn.h"
#include "Pawns/HugoPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Grid/GridManager.h"
#include "GS_PlagueTaleGo.h"
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

void AGM_PlagueTaleGo::OnPlayerMoveFinished() {
	if (!ensure(IsValid(PlagueGoGameState)) || !ensure(IsValid(GridManager))) {
		return;
	}

	if (PlagueGoGameState->CurrentGamePhase == EGamePhase::PlayerTurn) {
		PlagueGoGameState->SetNewGamePhase(EGamePhase::HugoTurn);
		if (IsPawnOnEnemyCell(GridManager->ActivePlayerPawn)) {
			HandleDefeat(GridManager->ActivePlayerPawn);
		}
		else {
			RunHugoTurn();
		}
	}
}

void AGM_PlagueTaleGo::OnExitReached(AActor* Actor) {
	if (!IsValid(Actor) || !ensure(IsValid(GridManager)) || !ensure(IsValid(PlagueGoGameState))) {
		return;
	}

	AMasterPawn* AllyPawn = Cast<AMasterPawn>(Actor);
	if (AllyPawn && AllyPawn->IsA<APlayerPawn>()) {
		UE_LOG(LogTemp, Warning, TEXT("Exit reached ! You win !"));
		GameOver();
	}
}

void AGM_PlagueTaleGo::BeginPlay() {
	Super::BeginPlay();

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (!ensure(IsValid(GridManager))) {
		return;
	}
}

void AGM_PlagueTaleGo::InitGameState() {
	Super::InitGameState();
	PlagueGoGameState = GetGameState<AGS_PlagueTaleGo>();
	ensure(IsValid(PlagueGoGameState));
}

void AGM_PlagueTaleGo::GameOver() {
	PlagueGoGameState->SetNewGamePhase(EGamePhase::GameOver);
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void AGM_PlagueTaleGo::RunEnemiesTurn() {
	for (AMasterPawn* Enemy : GridManager->ActiveEnemies) {
		if (!IsValid(Enemy)) continue;
		Enemy->ExecutePawnTurn();
		if (IsPawnCaughtBy(GridManager->ActivePlayerPawn, Enemy)) {
			HandleDefeat(GridManager->ActivePlayerPawn);
			return;
		}
		if (IsPawnCaughtBy(GridManager->ActiveHugoPawn, Enemy)) {
			HandleDefeat(GridManager->ActiveHugoPawn);
			return;
		}
	}
	OnEnemiesMoveFinished();
}

void AGM_PlagueTaleGo::OnEnemiesMoveFinished() {
	if (PlagueGoGameState->CurrentGamePhase == EGamePhase::EnemiesTurn) {
		PlagueGoGameState->SetNewGamePhase(EGamePhase::PlayerTurn);
	}
}

void AGM_PlagueTaleGo::RunHugoTurn() {
	/*GridManager->ActiveHugoPawn->ExecutePawnTurn();
	if (IsPawnOnEnemyCell(GridManager->ActiveHugoPawn)) {
		HandleDefeat(GridManager->ActiveHugoPawn);
	}*/
	OnHugoMoveFinished();
}

void AGM_PlagueTaleGo::OnHugoMoveFinished() {
	if (PlagueGoGameState->CurrentGamePhase == EGamePhase::HugoTurn) {
		PlagueGoGameState->SetNewGamePhase(EGamePhase::EnemiesTurn);
		if (IsPawnOnEnemyCell(GridManager->ActiveHugoPawn)) {
			HandleDefeat(GridManager->ActiveHugoPawn);
		}
		else {
			RunEnemiesTurn();
		}
	}
}

void AGM_PlagueTaleGo::HandleDefeat(AMasterPawn* DefeatedPawn) {
	if (!ensure(IsValid(DefeatedPawn))) {
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("%s has been caught ! Game Over."), *DefeatedPawn->GetName());
	GameOver();
}

bool AGM_PlagueTaleGo::IsPawnCaughtBy(AMasterPawn* FriendlyPawn, AMasterPawn* EnemyPawn) const {
	if (!IsValid(FriendlyPawn) || !IsValid(EnemyPawn)) {
		return false;
	}
	return (FriendlyPawn->CurrentGridPosition == EnemyPawn->CurrentGridPosition);
}

bool AGM_PlagueTaleGo::IsPawnOnEnemyCell(AMasterPawn* PawnToCheck) const {
	for (AMasterPawn* Enemy : GridManager->ActiveEnemies) {
		if (IsPawnCaughtBy(PawnToCheck, Enemy)) {
			return true;
		}
	}
	return false;
}