// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCardData.h"
#include "PlayerDeckState.generated.h"

USTRUCT(BlueprintType)
struct FPlayerDeckState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		TArray<UUCardData*> Deck;

	UPROPERTY(BlueprintReadOnly)
		TArray<UUCardData*> Hand;

	UPROPERTY(BlueprintReadOnly)
		TArray<UUCardData*> Discard;

	UPROPERTY(BlueprintReadOnly)
		int32 playerId = 0;
};