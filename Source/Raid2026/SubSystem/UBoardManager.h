#pragma once

#include "CoreMinimal.h"
#include "Raid2026/CoreLayer/FCell.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UBoardManager.generated.h"

UCLASS()
class RAID2026_API UUBoardManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	static constexpr int32 GridWidth = 5;
	static constexpr int32 GridHeight = 7;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVictoryConditionMet, int32, WinnerPlayerID);

	UPROPERTY(BlueprintAssignable)
	FOnVictoryConditionMet OnVictoryConditionMet;

	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRefineryControlChanged, ARefineryActor*, Refinery, int32, NewOwnerID);

	//UPROPERTY(BlueprintAssignable)
	//FOnRefineryControlChanged OnRefineryControlChanged;

	//UFUNCTION(BlueprintCallable)
	//void InitializeBoard(const TArray<ARefineryActor*>& Refineries, AMothershipActor* MothershipP0,AMothershipActor* MothershipP1);

	UFUNCTION(BlueprintPure)
	bool IsValidCell(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	bool IsCellOccupied(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	FCell GetCell(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	FCell GetCell(int32 X, int32 Y) const;

	//UFUNCTION(BlueprintPure)
	//AShipActor* GetShipAt(FIntPoint Pos) const;

	// UFUNCTION(BlueprintPure)
	// AMothershipActor* GetMothershipAt(FIntPoint Pos) const;

	UFUNCTION(BlueprintPure)
	bool IsMothershipCell(FIntPoint Pos, int32 ShooterPlayerID) const;

	UFUNCTION(BlueprintPure)
	TArray<FIntPoint> GetFreeSpawnCells(int32 PlayerID) const;

	UFUNCTION(BlueprintCallable)
	TArray<FReachableCell> GetReachableCells(AShipActor* Ship, int32 AvailableEssence) const;

	UFUNCTION(BlueprintPure)
	bool IsLineOfSight(FIntPoint From, FIntPoint To) const;

	UFUNCTION(BlueprintCallable)
	int32 CheckRefineries();

	//UFUNCTION(BlueprintCallable)
	//bool MoveShipTo(AShipActor* Ship, FIntPoint TargetCell);

	//UFUNCTION(BlueprintCallable)
	//void RemoveShipFromGrid(AShipActor* Ship);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	FCell Grid[GridWidth][GridHeight];

	//TArray<ARefineryActor*> RefineryActors;

	//TMap<int32, AMothershipActor*> Motherships;

	FCell& GetCellRef(FIntPoint Pos);

	//void SetOccupant(FIntPoint Pos, AHordeActor* Actor);

	void ClearOccupant(FIntPoint Pos);

	static const TArray<FIntPoint> OrthoDirections;
};


