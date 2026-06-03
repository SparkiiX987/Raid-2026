// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FReachableCell.generated.h"

USTRUCT(BlueprintType)
struct FReachableCell
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FIntPoint Cell;

	UPROPERTY(BlueprintReadOnly)
	int32 EssenceCost;
};