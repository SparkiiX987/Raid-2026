// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FEffectContext.generated.h"

USTRUCT(BlueprintType)
struct FEffectContext
{
	GENERATED_BODY()

	// UPROPERTY(BlueprintReadWrite)
	// TWeakObjectPtr<AShipActor> SourceShip;

	UPROPERTY(BlueprintReadWrite)
	int32 OwnerPlayerID;
	
	// UPROPERTY(BlueprintReadWrite)
	// TWeakObjectPtr<AShipActor> TargetShip;

	// UPROPERTY(BlueprintReadWrite)
	// TWeakObjectPtr<AMothershipActor> TargetMothership;

	UPROPERTY(BlueprintReadWrite)
	FIntPoint TargetCell;
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentTurn;

	UPROPERTY(BlueprintReadWrite)
	int32 EssencePaid;

	UPROPERTY(BlueprintReadWrite)
	int32 DamageDealt;

	// UBoardManager* Board;
	//
	// UTurnManager* Turn;
	//
	// UDeckManager* Deck;
};
