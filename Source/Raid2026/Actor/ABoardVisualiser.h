// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABoardCell.h"
#include "GameFramework/Actor.h"
#include "ABoardVisualiser.generated.h"

UCLASS()
class RAID2026_API AABoardVisualiser : public AActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellSize = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellGap = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AABoardCell> CellActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> RefineryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> MothershipClass;

	UFUNCTION(BlueprintCallable)
	void SpawnBoard(TArray<AActor*>& OutRefineries,AActor*& OutMothershipP0, AActor*& OutMothershipP1);

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

	void SpawnRefineries(TArray<AActor*>& OutRefineries);

	void SpawnMotherships(AActor*& OutP0, AActor*& OutP1);

	UFUNCTION(BlueprintCallable)
	void SpawnShip(TSubclassOf<AActor> Ship);

	int32 CellIndex(int32 X, int32 Y) const;
};