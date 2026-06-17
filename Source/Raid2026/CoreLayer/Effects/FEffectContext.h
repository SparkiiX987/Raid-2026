#pragma once

#include "CoreMinimal.h"
#include "../../SubSystem/UBoardManager.h"
#include "../../SubSystem/UTurnManager.h"
#include "../../SubSystem/DeckManager.h"
#include "FEffectContext.generated.h"

class UCombatResolver;

USTRUCT(BlueprintType)
struct FEffectContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		TWeakObjectPtr<AAShip> SourceShip;

	UPROPERTY(BlueprintReadWrite)
	int32 OwnerPlayerID;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AAShip> TargetShip;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AAMotherShip> TargetMothership;

	UPROPERTY(BlueprintReadWrite)
	FIntPoint TargetCell;
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentTurn;

	UPROPERTY(BlueprintReadWrite)
	int32 EssencePaid;

	UPROPERTY(BlueprintReadWrite)
	int32 DamageDealt;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUBoardManager> Board;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUTurnManager> Turn;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UDeckManager> Deck;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCombatResolver> Resolver;
};
