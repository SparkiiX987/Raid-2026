#pragma once

#include "CoreMinimal.h"
#include "ECellType.h"
#include "../../Actor/AShip.h"
#include "../../Actor/ARefinery.h"
#include "FCell.generated.h"

USTRUCT(BlueprintType)
struct FCell
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FIntPoint Pos = FIntPoint(0, 0);
	
	UPROPERTY(BlueprintReadOnly)
	ECellType Type;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AABoardActor> Occupant;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AARefinery> refinery;
	
	
	bool IsEmpty()
	{
		return !IsValid(Occupant);
	}
};
