#include "MasterPawn.h"
#include "Grid/GridManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

AMasterPawn::AMasterPawn() {
	PrimaryActorTick.bCanEverTick = false;

	PawnStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnStaticMesh"));
	SetRootComponent(PawnStaticMesh);

	SelectionCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SelectionCollision"));
	SelectionCollision->SetupAttachment(PawnStaticMesh);
	SelectionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SelectionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SelectionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	BaseBottom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseBottom"));
	BaseBottom->SetupAttachment(PawnStaticMesh);
	BaseBottom->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BaseBottom->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	BaseBottom->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	BaseBottom->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BaseTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseTop"));
	BaseTop->SetupAttachment(PawnStaticMesh);
	BaseTop->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f));
	BaseTop->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	BaseTop->SetRelativeScale3D(FVector(0.85f, 0.85f, 1.0f));
	BaseTop->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(PawnStaticMesh);
	CharacterMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 6.0f));
	CharacterMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	CharacterMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 0.55f));
	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMasterPawn::BeginPlay() {
	Super::BeginPlay();
}

void AMasterPawn::InitializePawnOnGrid(FIntVector2 StartPosition, EGridDirection StartDirection, AGridManager* Manager) {
	CurrentGridPosition = StartPosition;
	GridManager = Manager;
	SetFacingDirection(StartDirection);
}

void AMasterPawn::ExecutePawnTurn() {}

bool AMasterPawn::CanMoveToCell(FIntVector2 TargetCellCoords) const {
	if (!ensure(IsValid(GridManager))) {
		return false;
	}
	return GridManager->CanMoveBetweenCells(CurrentGridPosition, TargetCellCoords, const_cast<AMasterPawn*>(this));
}

bool AMasterPawn::CanMoveInDirection(EGridDirection Direction, FIntVector2& OutTargetCell) const {
	if (!ensure(IsValid(GridManager))) {
		return false;
	}
	return GridManager->CanMoveInDirection(CurrentGridPosition, Direction, const_cast<AMasterPawn*>(this), OutTargetCell);
}

void AMasterPawn::SetFacingDirection(EGridDirection NewDirection) {
	CurrentDirection = NewDirection;
	if (!ensure(IsValid(GridManager))) {
		return;
	}
	SetActorRotation(GridManager->DirectionToWorldRotation(NewDirection));
}

void AMasterPawn::TurnAround() {
	SetFacingDirection(AGridManager::GetOppositeDirection(CurrentDirection));
}

bool AMasterPawn::MoveToCell(FIntVector2 TargetCellCoords) {
	if (!ensure(IsValid(GridManager))) {
		return false;
	}

	if (!GridManager->CanMoveBetweenCells(CurrentGridPosition, TargetCellCoords, this)) {
		return false;
	}

	EGridDirection MoveDirection;
	if (GridManager->GetDirectionBetweenCells(CurrentGridPosition, TargetCellCoords, MoveDirection)) {
		SetFacingDirection(MoveDirection);
	}

	FVector2D TraversedPathKey = GridManager->GetPathCoordsKey(CurrentGridPosition, TargetCellCoords);
	GridManager->TriggerPath(TraversedPathKey, this);

	CurrentGridPosition = TargetCellCoords;
	SetActorLocation(GridManager->GridToWorldLocation(TargetCellCoords));

	GridManager->TriggerCell(TargetCellCoords, this);

	return true;
}

bool AMasterPawn::MoveInDirection(EGridDirection Direction, bool bTurnAroundAtDeadEnd) {
	if (!ensure(IsValid(GridManager))) {
		return false;
	}

	SetFacingDirection(Direction);

	FIntVector2 TargetCell;
	if (!CanMoveInDirection(Direction, TargetCell)) {
		if (bTurnAroundAtDeadEnd) {
			TurnAround();
		}
		return false;
	}

	if (MoveToCell(TargetCell)) {
		FIntVector2 NextCellCheck;
		if (bTurnAroundAtDeadEnd && !CanMoveInDirection(CurrentDirection, NextCellCheck)) {
			TurnAround();
		}
		return true;
	}

	return false;
}

bool AMasterPawn::StepForwardPatrol() {
	if (!ensure(IsValid(GridManager))) {
		return false;
	}

	FIntVector2 TargetCell;
	if (!CanMoveInDirection(CurrentDirection, TargetCell)) {
		TurnAround();
	}

	return MoveInDirection(CurrentDirection, true);
}

void AMasterPawn::TeleportToCell(FIntVector2 TargetCellCoords, bool bTriggerDestinationCell) {
	if (!ensure(IsValid(GridManager))) {
		return;
	}

	CurrentGridPosition = TargetCellCoords;
	SetActorLocation(GridManager->GridToWorldLocation(TargetCellCoords));

	if (bTriggerDestinationCell) {
		GridManager->TriggerCell(TargetCellCoords, this);
	}
}