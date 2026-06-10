#pragma once

#include "CoreMinimal.h"
#include "ReplicatedCellState.generated.h"

USTRUCT(BlueprintType)
struct RAID2026_API FReplicatedCellState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		FIntPoint pos;

	UPROPERTY(BlueprintReadOnly)
		int32 OccupantPlayerId = -1;

	UPROPERTY(BlueprintReadOnly)
		bool bHasShip = false;

	UPROPERTY(BlueprintReadOnly)
		bool bHasMothership = false;

	UPROPERTY(BlueprintReadOnly)
		bool bHasRefinery = false;

	UPROPERTY(BlueprintReadOnly)
		int32 RefineryOwnerId = -1;
};
