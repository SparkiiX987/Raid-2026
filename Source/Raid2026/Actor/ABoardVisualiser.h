// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABoardCell.h"
#include "ARefinery.h"
#include "GameFramework/Actor.h"
#include "Raid2026/SubSystem/UBoardManager.h"
#include "Raid2026/SubSystem/UTurnManager.h"
#include "ABoardVisualiser.generated.h"

UCLASS()
class RAID2026_API AABoardVisualiser : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellSize = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UUTurnManager> TurnManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UUBoardManager> BoardManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AABoardCell> CellActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AARefinery> RefineryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAMotherShip> MothershipClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAShip> ShipClass;

	UFUNCTION(BlueprintCallable)
	void SpawnBoard(TArray<AARefinery*>& OutRefineries,AAMotherShip*& OutMothershipP0, AAMotherShip*& OutMothershipP1);

	UFUNCTION(BlueprintCallable)
	void DebugSpawnShipInAllCells();

	UFUNCTION(BlueprintPure)
	AABoardCell* GetCellActor(FIntPoint GridPos) const;

	UFUNCTION(BlueprintPure)
	FVector GridToWorld(FIntPoint GridPos) const;

	UFUNCTION(BlueprintPure)
	FIntPoint WorldToGrid(FVector WorldPos) const;

	TArray<AABoardCell*> CellActors;

	static const TArray<FIntPoint> RefineryGridPositions;

	void SpawnCellActors();

	void SpawnRefineries(TArray<AARefinery*>& OutRefineries);

	void SpawnMotherships(AAMotherShip*& OutP0, AAMotherShip*& OutP1);

	UFUNCTION(BlueprintCallable)
	AAShip* SpawnShip(TSubclassOf<AAShip> Ship, FIntPoint GridPos, UUCardData* cardData, int32 PlayerID);

	int32 CellIndex(int32 X, int32 Y) const;
};