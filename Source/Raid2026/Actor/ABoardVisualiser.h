#pragma once

#include "CoreMinimal.h"
#include "ABoardCell.h"
#include "ARefinery.h"
#include "GameFramework/Actor.h"
#include "Raid2026/SubSystem/UBoardManager.h"
#include "Raid2026/SubSystem/UTurnManager.h"
#include "../CoreLayer/Cells/ReplicatedCellState.h"
#include "ABoardVisualiser.generated.h"

UCLASS()
class RAID2026_API AABoardVisualiser : public AActor
{
	GENERATED_BODY()
	
public:
	AABoardVisualiser();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellSize = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UUTurnManager> TurnManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UUBoardManager> BoardManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AARefinery> RefineryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAMotherShip> MothershipClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAShip> ShipClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AABoardCell> CellActorClass;

	UPROPERTY()
		TArray<AABoardCell*> highlitedCells;

	void TryBindToGameState();

	UFUNCTION(BlueprintPure)
		AABoardCell* GetCellActor(FIntPoint GridPos) const;

	UFUNCTION(BlueprintPure)
		FVector GridToWorld(FIntPoint GridPos) const;

	UFUNCTION(BlueprintPure)
		FIntPoint WorldToGrid(FVector WorldPos) const;

	UFUNCTION()
		void HighlightCells(TArray<FIntPoint> Cells);

	UFUNCTION()
		void ClearHighlights();

	void OnGridStateChanged(const TArray<FReplicatedCellState>& NewGrid);

	void OnActivePlayerChanged(int32 newActivePlayer);

	void OnGameOver(int32 winner);

	UPROPERTY(replicated)
	TArray<AABoardCell*> CellActors;

	static const TArray<FIntPoint> RefineryGridPositions;

	void SpawnCellActors();

	int32 CellIndex(int32 X, int32 Y) const;
};