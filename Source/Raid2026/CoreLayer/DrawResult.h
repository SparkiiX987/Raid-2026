// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCardData.h"
#include "DrawResult.generated.h"

USTRUCT(BlueprintType)
struct FDrawResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		UUCardData* DrawnCard = nullptr;
			
	UPROPERTY(BlueprintReadOnly)
		bool bDeckEmpty = false;

	UPROPERTY(BlueprintReadOnly)
		int32 DamageTaken = 0;
};
