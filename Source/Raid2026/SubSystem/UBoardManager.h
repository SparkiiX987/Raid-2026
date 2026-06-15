#pragma once

#include "CoreMinimal.h"
#include "UTurnManager.h"
#include "../CoreLayer/Cells/FCell.h"
#include "Raid2026/Actor/AMotherShip.h"
#include "Raid2026/Actor/AShip.h"
#include "UBoardManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UUBoardManager : public UObject
{
	GENERATED_BODY()
	
public:
	static constexpr int32 GridWidth = 5;
	static constexpr int32 GridHeight = 7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellGap = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UUTurnManager> TurnManager;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVictoryConditionMet, int32, WinnerPlayerID);

	UPROPERTY(BlueprintAssignable)
	FOnVictoryConditionMet OnVictoryConditionMet;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRefineryControlChanged, AARefinery*, Refinery, int32, NewOwnerID);

	UPROPERTY(BlueprintAssignable)
	FOnRefineryControlChanged OnRefineryControlChanged;

	UFUNCTION(BlueprintCallable)
	void InitializeBoard(const TArray<AARefinery*>& Refineries, AAMotherShip* MothershipP0,AAMotherShip* MothershipP1);

	UFUNCTION(BlueprintPure)
	bool IsValidCell(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	bool IsCellOccupied(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	FCell GetCell(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	FCell GetCells(int32 X, int32 Y) const;

	UFUNCTION(BlueprintPure)
	AAShip* GetShipAt(FIntPoint Pos) const;
	
	 UFUNCTION(BlueprintPure)
	 AAMotherShip* GetMothershipAt(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	bool IsMothershipCell(FIntPoint Pos, int32 ShooterPlayerID) const;

	UFUNCTION(BlueprintPure)
	TArray<FIntPoint> GetFreeSpawnCells(int32 PlayerID) const;

	UFUNCTION(BlueprintCallable)
	TArray<FReachableCell> GetReachableCells(AAShip* Ship, int32 AvailableEssence) const;
	
	UFUNCTION(BlueprintPure)
	bool IsLineOfSight(FIntPoint From, FIntPoint To) const;

	UFUNCTION(BlueprintCallable)
	int32 CheckRefineries(int32 playerID);

	UFUNCTION(BlueprintCallable)
	bool MoveShipTo(AAShip* Ship, FIntPoint TargetCell,int32 playerID);
	
	UFUNCTION(BlueprintCallable)
	void PlaceShip(AAShip* Ship, FIntPoint target);
	
	UFUNCTION(BlueprintCallable)
	void RemoveShipFromGrid(AAShip* Ship);

	FCell Grid[GridWidth][GridHeight+2];

	TArray<AARefinery*> RefineryActors;
	
	TMap<int32, AAMotherShip*> Motherships;

	FCell& GetCellRef(FIntPoint Pos);

	void SetOccupant(FIntPoint Pos, AABoardActor* Actor);

	void ClearOccupant(FIntPoint Pos);

	static const TArray<FIntPoint> OrthoDirections;
};


