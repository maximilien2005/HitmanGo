#include "GridManager.h"
#include "Pawns/MasterPawn.h"
#include "Components/ChildActorComponent.h"
#include "Components/TextRenderComponent.h"

AGridManager::AGridManager() {
	PrimaryActorTick.bCanEverTick = false;

	// Main Root Component is the board
	Board = CreateDefaultSubobject<USceneComponent>(TEXT("Board"));
	SetRootComponent(Board);

	Cells = CreateDefaultSubobject<USceneComponent>(TEXT("Cells")); // Hold all the cell instances
	Cells->SetupAttachment(Board);

	Paths = CreateDefaultSubobject<USceneComponent>(TEXT("Paths")); // Hold all the path instances
	Paths->SetupAttachment(Board);

	CellInstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CellInstancedMesh"));
	CellInstancedMesh->SetupAttachment(Cells);

	PathInstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathInstancedMesh"));
	PathInstancedMesh->SetupAttachment(Paths);

	HighlightInstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("HighlightInstancedMesh"));
	HighlightInstancedMesh->SetupAttachment(Cells);
	HighlightInstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

#if WITH_EDITORONLY_DATA
	Previews = CreateDefaultSubobject<USceneComponent>(TEXT("Previews")); // Hold all the preview components (player and enemies)
	if (IsValid(Previews)) {
		Previews->SetupAttachment(Board);
	}

	EnemiesPreview = CreateDefaultSubobject<USceneComponent>(TEXT("EnemiesPreview")); // Hold all the enemy preview components
	if (IsValid(EnemiesPreview)) {
		EnemiesPreview->SetupAttachment(Previews);
	}

	PlayersPreview = CreateDefaultSubobject<USceneComponent>(TEXT("PlayersPreview")); // Hold all the player preview components
	if (IsValid(PlayersPreview)) {
		PlayersPreview->SetupAttachment(Previews);
	}
#endif
}

void AGridManager::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	if (!ensure(IsValid(CellInstancedMesh)) || !ensure(IsValid(PathInstancedMesh)) || !ensure(IsValid(HighlightInstancedMesh))) {
		return;
	}

	for (auto& Pair : RuntimeSpecialPaths) {
		if (IsValid(Pair.Value)) {
			Pair.Value->DestroyComponent();
		}
	}

	for (auto& Pair : RuntimeSpecialCases) {
		if (IsValid(Pair.Value)) {
			Pair.Value->DestroyComponent();
		}
	}

	CellInstancedMesh->ClearInstances();
	PathInstancedMesh->ClearInstances();
	HighlightInstancedMesh->ClearInstances();
	RuntimeSpecialCases.Empty();
	RuntimeSpecialPaths.Empty();

#if WITH_EDITORONLY_DATA
	TArray<UTextRenderComponent*> ExistingTexts;
	GetComponents<UTextRenderComponent>(ExistingTexts);
	for (UTextRenderComponent* TextComp : ExistingTexts) {
		if (IsValid(TextComp)) {
			TextComp->DestroyComponent();
		}
	}
	TextPool.Empty();

	ClearGridPreview();
#endif

	if (!IsValid(GridData)) {
		return;
	}

#if WITH_EDITOR
	for (int coord_x = 0; coord_x < GridData->MapSize.X; coord_x++) {
		for (int coord_y = 0; coord_y < GridData->MapSize.Y; coord_y++) {
			FIntVector2 CurrentPos(coord_x, coord_y);
			
			FVector InstanceLocation = GridToLocalLocation(CurrentPos);
			FTransform InstanceTransform(InstanceLocation);

			if (!GridData->listSpecialCase.Contains(CurrentPos)) {
				CellInstancedMesh->AddInstance(InstanceTransform);
			}

			if (bLookCoords) {
				UTextRenderComponent* TextRender = NewObject<UTextRenderComponent>(this);
				if (!ensure(IsValid(TextRender))) {
					continue;
				}
				TextRender->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				TextRender->SetupAttachment(Cells);
				TextRender->RegisterComponent();
				TextPool.Add(TextRender);

				TextRender->SetVisibility(true);
				TextRender->SetRelativeLocation(InstanceLocation + FVector(0.0f, 0.0f, 5.0f));
				TextRender->SetRelativeRotation(FRotator(90.0f, 180.0f, 0.0f));
				TextRender->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
				TextRender->SetHorizontalAlignment(EHTA_Center);
				TextRender->SetVerticalAlignment(EVRTA_TextCenter);
				TextRender->SetText(FText::FromString(FString::Printf(TEXT("(%d, %d)"), coord_x, coord_y)));
				TextRender->SetTextRenderColor(FColor(0, 0, 0, 255));
			}
		}
	}
#endif

	for (const TPair<FIntVector2, FSpecialCaseConfig>& ConfigPair : GridData->listSpecialCase) {
		TSubclassOf<UMainCaseComponent> CellClass = ConfigPair.Value.CaseClass;
		if (!CellClass) {
			continue;
		}

		UMainCaseComponent* NewComp = NewObject<UMainCaseComponent>(this, CellClass);
		if (!ensure(IsValid(NewComp))) {
			continue;
		}

		NewComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		NewComp->SetupAttachment(Cells);
		NewComp->GridManagerRef = this;

		FVector InstanceLocation = GridToLocalLocation(ConfigPair.Key);
		NewComp->SetRelativeLocation(InstanceLocation);

		NewComp->RegisterComponent();
		RuntimeSpecialCases.Add(ConfigPair.Key, NewComp);
	}

	for (const TPair<FVector2D, FChemainStruct>& ChainDataPair : GridData->listChemainData) {
		FVector InstanceLocation(ChainDataPair.Key.X * EspaceCells, ChainDataPair.Key.Y * EspaceCells, 0.0f);

		FRotator InstanceRotation = FRotator::ZeroRotator;
		if (ChainDataPair.Value.GridCellPositionA.Y == ChainDataPair.Value.GridCellPositionB.Y) {
			InstanceRotation = FRotator(0.0f, 90.0f, 0.0f);
		}

		FTransform InstanceTransform(InstanceRotation, InstanceLocation, FVector(1.0f));
		TSubclassOf<UMainPathComponent> PathClass = ChainDataPair.Value.SpecialPathClass;

		if (PathClass) {
			UMainPathComponent* NewPathComp = NewObject<UMainPathComponent>(this, PathClass);
			if (!ensure(IsValid(NewPathComp))) {
				continue;
			}

			NewPathComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			NewPathComp->SetupAttachment(Paths);
			NewPathComp->GridManagerRef = this;
			NewPathComp->PathCoordinates = ChainDataPair.Key;
			NewPathComp->GridCellPositionA = ChainDataPair.Value.GridCellPositionA;
			NewPathComp->GridCellPositionB = ChainDataPair.Value.GridCellPositionB;
			NewPathComp->SetRelativeTransform(InstanceTransform);

			NewPathComp->RegisterComponent();
			RuntimeSpecialPaths.Add(ChainDataPair.Key, NewPathComp);
		}
		else {
			PathInstancedMesh->AddInstance(InstanceTransform);
		}
	}

#if WITH_EDITOR
	if (!GetWorld()->IsGameWorld()) {
		BuildPlayerPreview();
		BuildEnemiesPreview();
	}
#endif
}

void AGridManager::BeginPlay() {
	Super::BeginPlay();

	ClearCellHighlights();

	TArray<UTextRenderComponent*> TextComponents;
	GetComponents<UTextRenderComponent>(TextComponents);
	for (UTextRenderComponent* TextComp : TextComponents) {
		if (IsValid(TextComp)) {
			TextComp->DestroyComponent();
		}
	}
	
#if WITH_EDITOR
	ClearGridPreview();
#endif

	if (!ensure(IsValid(GridData))) {
		return;
	}

	TArray<FIntVector2> UsedCells;
	for (const TPair<FVector2D, FChemainStruct>& ChainDataPair : GridData->listChemainData) {
		UsedCells.AddUnique(ChainDataPair.Value.GridCellPositionA);
		UsedCells.AddUnique(ChainDataPair.Value.GridCellPositionB);
	}

	if (IsValid(CellInstancedMesh)) {
		CellInstancedMesh->ClearInstances();
		for (const FIntVector2& CellPos : UsedCells) {
			if (!GridData->listSpecialCase.Contains(CellPos)) {
				FVector InstanceLocation = GridToLocalLocation(CellPos);
				CellInstancedMesh->AddInstance(FTransform(InstanceLocation));
			}
		}
	}

	ActivePlayerPawn = nullptr;
	if (GridData->PlayerClass != nullptr && GetWorld() != nullptr) {
		FVector PlayerWorldLocation = GridToWorldLocation(GridData->PlayerStart);
		FRotator PlayerWorldRotation = DirectionToWorldRotation(GridData->PlayerStartDirection);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ActivePlayerPawn = GetWorld()->SpawnActor<AMasterPawn>(GridData->PlayerClass, PlayerWorldLocation, PlayerWorldRotation, SpawnParams);
		if (IsValid(ActivePlayerPawn)) {
			ActivePlayerPawn->InitializePawnOnGrid(GridData->PlayerStart, GridData->PlayerStartDirection, this);
		}
	}

	ActiveEnemies.Empty();
	if (GetWorld() != nullptr) {
		for (const TPair<FIntVector2, FEnemySpawnData>& EnemyPair : GridData->listEnemiesStart) {
			if (EnemyPair.Value.PawnClass == nullptr) {
				continue;
			}

			FVector EnemyWorldLocation = GridToWorldLocation(EnemyPair.Key);
			FRotator EnemyWorldRotation = DirectionToWorldRotation(EnemyPair.Value.StartDirection);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AMasterPawn* SpawnedEnemy = GetWorld()->SpawnActor<AMasterPawn>(EnemyPair.Value.PawnClass, EnemyWorldLocation, EnemyWorldRotation, SpawnParams);
			if (IsValid(SpawnedEnemy)) {
				SpawnedEnemy->InitializePawnOnGrid(EnemyPair.Key, EnemyPair.Value.StartDirection, this);
				ActiveEnemies.Add(SpawnedEnemy);
			}
		}
	}
}

#if WITH_EDITOR
void AGridManager::ClearGridPreview() {
	ClearPreviewGroup(PlayersPreview);
	ClearPreviewGroup(EnemiesPreview);
}

void AGridManager::ClearPreviewGroup(USceneComponent* ParentGroup) {
	if (!IsValid(ParentGroup)) {
		return;
	}

	TArray<USceneComponent*> Childs;
	ParentGroup->GetChildrenComponents(false, Childs);

	for (USceneComponent* Child : Childs) {
		if (UChildActorComponent* PreviewComp = Cast<UChildActorComponent>(Child)) {
			if (IsValid(PreviewComp)) {
				PreviewComp->DestroyComponent();
			}
		}
	}
}


UChildActorComponent* AGridManager::CreatePawnPreview(USceneComponent* ParentGroup, TSubclassOf<AMasterPawn> PawnClass, FIntVector2 CellCoords, EGridDirection Direction) {
	if (!IsValid(ParentGroup) || !IsValid(PawnClass)) {
		return nullptr;
	}

	UChildActorComponent* PreviewComp = NewObject<UChildActorComponent>(this);
	PreviewComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	PreviewComp->SetupAttachment(ParentGroup);
	PreviewComp->SetChildActorClass(PawnClass);
	PreviewComp->SetRelativeLocation(GridToLocalLocation(CellCoords));
	PreviewComp->SetRelativeRotation(DirectionToLocalRotation(Direction));
	PreviewComp->RegisterComponent();

	return PreviewComp;
}

void AGridManager::BuildPlayerPreview() {
	if (!IsValid(GridData)) {
		return;
	}
	CreatePawnPreview(PlayersPreview, GridData->PlayerClass, GridData->PlayerStart, GridData->PlayerStartDirection);
}

void AGridManager::BuildEnemiesPreview() {
	if (!IsValid(GridData)) {
		return;
	}

	for (const TPair<FIntVector2, FEnemySpawnData>& EnemyPair : GridData->listEnemiesStart) {
		if (EnemyPair.Value.PawnClass == nullptr) {
			continue;
		}

		CreatePawnPreview(EnemiesPreview, EnemyPair.Value.PawnClass, EnemyPair.Key, EnemyPair.Value.StartDirection);
	}
}

#endif

FVector2D AGridManager::GetPathCoordsKey(FIntVector2 GridCellPositionA, FIntVector2 GridCellPositionB) const {
	if (IsValid(GridData)) {
		return GridData->GetPathCoordsKey(GridCellPositionA, GridCellPositionB);
	}
	return FVector2D(
		(GridCellPositionA.X + GridCellPositionB.X) / 2.0f,
		(GridCellPositionA.Y + GridCellPositionB.Y) / 2.0f
	);
}

FVector AGridManager::GridToWorldLocation(FIntVector2 CellCoords) const {
	return GetActorTransform().TransformPosition(GridToLocalLocation(CellCoords));
}

FVector AGridManager::GridToLocalLocation(FIntVector2 CellCoords) const {
	FVector LocalLocation(CellCoords.X * EspaceCells, CellCoords.Y * EspaceCells, 0.0f);
	return LocalLocation;
}

bool AGridManager::WorldToGridLocation(FVector WorldLocation, FIntVector2& OutCellCoords, float MaxDistanceTolerance) const {
	if (EspaceCells <= 0.0f) {
		return false;
	}

	FVector LocalLocation = GetActorTransform().InverseTransformPosition(WorldLocation);
	int32 ClosestX = FMath::RoundToInt(LocalLocation.X / EspaceCells);
	int32 ClosestY = FMath::RoundToInt(LocalLocation.Y / EspaceCells);

	FVector CellCenterLocal = GridToLocalLocation(FIntVector2(ClosestX, ClosestY));
	float Distance2D = FVector::Dist2D(LocalLocation, CellCenterLocal);

	if (Distance2D <= MaxDistanceTolerance) {
		OutCellCoords = FIntVector2(ClosestX, ClosestY);
		return true;
	}

	return false;
}

UMainPathComponent* AGridManager::GetSpecialPathBetween(FIntVector2 CellA, FIntVector2 CellB) const {
	FVector2D PathKey = GetPathCoordsKey(CellA, CellB);
	if (RuntimeSpecialPaths.Contains(PathKey)) {
		return RuntimeSpecialPaths[PathKey];
	}
	return nullptr;
}

bool AGridManager::CanMoveBetweenCells(FIntVector2 StartCell, FIntVector2 TargetCell, AActor* MovingActor) const {
	if (!ensure(IsValid(GridData)) || StartCell == TargetCell) {
		return false;
	}

	FVector2D PathKey = GetPathCoordsKey(StartCell, TargetCell);
	const FChemainStruct* FoundPath = GridData->listChemainData.Find(PathKey);

	if (!FoundPath) {
		return false;
	}

	bool bMatchesEndpoints =
		(FoundPath->GridCellPositionA == StartCell && FoundPath->GridCellPositionB == TargetCell) ||
		(FoundPath->GridCellPositionB == StartCell && FoundPath->GridCellPositionA == TargetCell);

	if (!bMatchesEndpoints) {
		return false;
	}

	if (FoundPath->SpecialPathClass != nullptr) {
		if (RuntimeSpecialPaths.Contains(PathKey) && IsValid(RuntimeSpecialPaths[PathKey])) {
			return RuntimeSpecialPaths[PathKey]->IsPraticable(MovingActor, StartCell);
		}
		return false;
	}

	return true;
}

TArray<FIntVector2> AGridManager::GetConnectedCells(FIntVector2 PositionCheck, AActor* MovingActor) const {
	TArray<FIntVector2> OutConnectedCells;
	if (!ensure(IsValid(GridData))) {
		return OutConnectedCells;
	}

	for (const TPair<FVector2D, FChemainStruct>& ChainDataPair : GridData->listChemainData) {
		FIntVector2 TargetCell;
		bool bIsConnected = false;

		if (ChainDataPair.Value.GridCellPositionA == PositionCheck) {
			TargetCell = ChainDataPair.Value.GridCellPositionB;
			bIsConnected = true;
		}
		else if (ChainDataPair.Value.GridCellPositionB == PositionCheck) {
			TargetCell = ChainDataPair.Value.GridCellPositionA;
			bIsConnected = true;
		}

		if (!bIsConnected) {
			continue;
		}

		if (CanMoveBetweenCells(PositionCheck, TargetCell, MovingActor)) {
			OutConnectedCells.AddUnique(TargetCell);
		}
	}
	return OutConnectedCells;
}

bool AGridManager::GetDirectionBetweenCells(FIntVector2 StartCell, FIntVector2 TargetCell, EGridDirection& OutDirection) const {
	int32 DeltaX = TargetCell.X - StartCell.X;
	int32 DeltaY = TargetCell.Y - StartCell.Y;

	if (DeltaX == 0 && DeltaY == 0) {
		return false;
	}

	if (FMath::Abs(DeltaX) >= FMath::Abs(DeltaY)) {
		if (DeltaX > 0) {
			OutDirection = EGridDirection::North;
		}
		else {
			OutDirection = EGridDirection::South;
		}
	}
	else {
		if (DeltaY > 0) {
			OutDirection = EGridDirection::East;
		}
		else {
			OutDirection = EGridDirection::West;
		}
	}
	return true;
}

bool AGridManager::CanMoveInDirection(FIntVector2 StartCell, EGridDirection Direction, AActor* MovingActor, FIntVector2& OutTargetCell) const {
	if (!ensure(IsValid(GridData))) {
		return false;
	}

	for (const TPair<FVector2D, FChemainStruct>& ChainDataPair : GridData->listChemainData) {
		FIntVector2 CandidateCell;
		bool bIsConnected = false;

		if (ChainDataPair.Value.GridCellPositionA == StartCell) {
			CandidateCell = ChainDataPair.Value.GridCellPositionB;
			bIsConnected = true;
		}
		else if (ChainDataPair.Value.GridCellPositionB == StartCell) {
			CandidateCell = ChainDataPair.Value.GridCellPositionA;
			bIsConnected = true;
		}

		if (!bIsConnected) {
			continue;
		}

		EGridDirection PathDirection;
		if (GetDirectionBetweenCells(StartCell, CandidateCell, PathDirection) && PathDirection == Direction) {
			if (CanMoveBetweenCells(StartCell, CandidateCell, MovingActor)) {
				OutTargetCell = CandidateCell;
				return true;
			}
		}
	}

	return false;
}

EGridDirection AGridManager::GetOppositeDirection(EGridDirection Direction) {
	switch (Direction) {
	case EGridDirection::North:
		return EGridDirection::South;
	case EGridDirection::East:
		return EGridDirection::West;
	case EGridDirection::South:
		return EGridDirection::North;
	case EGridDirection::West:
		return EGridDirection::East;
	default:
		return EGridDirection::North;
	}
}

FRotator AGridManager::DirectionToLocalRotation(EGridDirection Direction) {
	float LocalYaw = 0.0f;
	switch (Direction) {
	case EGridDirection::North:
		LocalYaw = 180.0f;
		break;
	case EGridDirection::East:
		LocalYaw = -90.0f;
		break;
	case EGridDirection::South:
		LocalYaw = 0.0f;
		break;
	case EGridDirection::West:
		LocalYaw = 90.0f;
		break;
	}
	return FRotator(0.0f, LocalYaw, 0.0f);
}

FRotator AGridManager::DirectionToWorldRotation(EGridDirection Direction) const {
	FRotator LocalRot = DirectionToLocalRotation(Direction);
	return GetActorTransform().TransformRotation(LocalRot.Quaternion()).Rotator();
}

EGridDirection AGridManager::WorldRotationToDirection(FRotator WorldRotation) const {
	FRotator LocalRot = GetActorTransform().InverseTransformRotation(WorldRotation.Quaternion()).Rotator();
	float NormalizedYaw = FRotator::NormalizeAxis(LocalRot.Yaw);

	if (NormalizedYaw >= -45.0f && NormalizedYaw < 45.0f) {
		return EGridDirection::South;
	}
	if (NormalizedYaw >= 45.0f && NormalizedYaw < 135.0f) {
		return EGridDirection::West;
	}
	if (NormalizedYaw >= -135.0f && NormalizedYaw < -45.0f) {
		return EGridDirection::East;
	}
	return EGridDirection::North;
}

void AGridManager::ShowCellHighlights(const TArray<FIntVector2>& CellsToHighlight) {
	if (!ensure(IsValid(HighlightInstancedMesh))) {
		return;
	}

	HighlightInstancedMesh->ClearInstances();

	for (const FIntVector2& CellPos : CellsToHighlight) {
		FVector LocalPos = GridToLocalLocation(CellPos);
		LocalPos.Z += 2.0f;
		HighlightInstancedMesh->AddInstance(FTransform(LocalPos));
	}
}

void AGridManager::ClearCellHighlights() {
	if (IsValid(HighlightInstancedMesh)) {
		HighlightInstancedMesh->ClearInstances();
	}
}

void AGridManager::TriggerCell(FIntVector2 CellCoords, AActor* TargetActor) {
	if (!ensure(IsValid(TargetActor))) {
		return;
	}

	if (RuntimeSpecialCases.Contains(CellCoords)) {
		UMainCaseComponent* CaseComp = RuntimeSpecialCases[CellCoords];
		if (IsValid(CaseComp)) {
			CaseComp->ExecuteCaseLogic(TargetActor, CellCoords);
		}
	}
}

void AGridManager::TriggerPath(FVector2D PathCoords, AActor* TargetActor) {
	if (!ensure(IsValid(TargetActor))) {
		return;
	}

	if (RuntimeSpecialPaths.Contains(PathCoords)) {
		UMainPathComponent* PathComp = RuntimeSpecialPaths[PathCoords];
		if (IsValid(PathComp)) {
			PathComp->ExecutePathLogic(TargetActor, PathCoords);
		}
	}
}