#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MainCaseComponent.h"
#include "MainPathComponent.h"
#include "GridDataAsset.generated.h"

class AMasterPawn;

UENUM(BlueprintType)
enum class EGridDirection : uint8 {
	North UMETA(DisplayName = "Haut (Nord / +X)"),
	East  UMETA(DisplayName = "Droite (Est / +Y)"),
	South UMETA(DisplayName = "Bas (Sud / -X)"),
	West  UMETA(DisplayName = "Gauche (Ouest / -Y)")
};

USTRUCT(BlueprintType)
struct FChemainStruct {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 GridCellPositionA = FIntVector2(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 GridCellPositionB = FIntVector2(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMainPathComponent> SpecialPathClass = nullptr;
};

USTRUCT(BlueprintType)
struct FEnemySpawnData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMasterPawn> PawnClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGridDirection StartDirection = EGridDirection::North;
};

USTRUCT(BlueprintType)
struct FSpecialCaseConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMainCaseComponent> CaseClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 TargetPosition = FIntVector2(0, 0);
};

UCLASS(BlueprintType)
class HITMANGO_API UGridDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParameters | Dimensions")
	FIntVector2 MapSize = FIntVector2(5, 5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParameters | Player")
	TSubclassOf<AMasterPawn> PlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParameters | Player")
	FIntVector2 PlayerStart = FIntVector2(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParameters | Player")
	EGridDirection PlayerStartDirection = EGridDirection::North;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParameters | Data")
	TMap<FVector2D, FChemainStruct> listChemainData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParameters | Data")
	TMap<FIntVector2, FEnemySpawnData> listEnemiesStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParameters | Data")
	TMap<FIntVector2, FSpecialCaseConfig> listSpecialCase;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector2D GetPathCoordsKey(FIntVector2 GridCellPositionA, FIntVector2 GridCellPositionB) const;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Path Tool")
	FIntVector2 PathStartCell = FIntVector2(0, 0);

	UPROPERTY(EditAnywhere, Category = "Path Tool")
	FIntVector2 PathEndCell = FIntVector2(0, 0);
#endif

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Path Tool")
	void GenerateEditorPath();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void RefreshConnectedGridManagers();
#endif
};