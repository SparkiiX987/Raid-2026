#pragma once

#include "CoreMinimal.h"
#include "UTurnManager.h"
#include "Raid2026/Actor/AShip.h"
#include "Raid2026/CoreLayer/Cells/FCell.h"
#include "UPathFinder.generated.h"

class UUBoardManager;

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UPathFinder : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UUTurnManager> TurnManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UUBoardManager> BoardManager;

	static const TArray<FIntPoint> Directions;

	static const TArray<FIntPoint> DiagonalDirections;

	int32 shipSpeed;
	FIntPoint cellSelect;

	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> PathTaken;

	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> ReachableCell;

	UPROPERTY(BlueprintReadOnly)
	int32 PathCost;

	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> InitializeCheck(AAShip* Ship, FCell CellToCheck);

	UFUNCTION(BlueprintCallable)
	void SearchPath(FIntPoint StartCell, AAShip* Ship,const TArray<FIntPoint>& DirectionsToUse);

	UFUNCTION(BlueprintCallable)
	void BuildPath(FIntPoint StartCell, const TMap<FIntPoint, FIntPoint>& Parent);

	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetAllCellAroundShip(AAShip* Ship);
};