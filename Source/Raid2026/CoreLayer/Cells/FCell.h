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

	UPROPERTY(BlueprintReadOnly)
	int32 isSpyByPlayer1;

	UPROPERTY(BlueprintReadOnly)
	int32 isSpyByPlayer2;
	
	
	bool IsEmpty()
	{
		return !IsValid(Occupant);
	}

	bool isSpyByThePlayer1()
	{
		return isSpyByPlayer1 > 0;
	}

	bool isSpyByThePlayer2()
	{
		return isSpyByPlayer2 > 0;
	}
};
