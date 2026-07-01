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
		TWeakObjectPtr<AABoardActor> SourceShip;

	UPROPERTY(BlueprintReadWrite)
	int32 OwnerPlayerID;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AABoardActor> TargetShip;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AAMotherShip> TargetMothership;

	UPROPERTY(BlueprintReadWrite)
	FIntPoint TargetCell;

	UPROPERTY(BlueprintReadWrite)
	TArray<FIntPoint> Directions {
		FIntPoint(1, 0),
	FIntPoint(-1, 0),
	FIntPoint(0, 1),
	FIntPoint(0, -1)
	};
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentTurn;

	UPROPERTY(BlueprintReadWrite)
	int32 EssencePaid;

	UPROPERTY(BlueprintReadWrite)
	int32 DamageDealt;

	UPROPERTY(BlueprintReadWrite)
	int32 EffectMaxHealth;

	UPROPERTY(BlueprintReadWrite)
	int32 EffectCurrentHealth;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UUCardData> discardedCard;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UUpgrade> targetedUpgrade;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UUCardData> targetedExpert;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUCardData> CardPlay;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUBoardManager> Board;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUTurnManager> Turn;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UDeckManager> Deck;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCombatResolver> Resolver;
};
