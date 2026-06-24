// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCardData.h"
#include "Engine/DataAsset.h"
#include "AllCards.generated.h"

UCLASS(BlueprintType)
class RAID2026_API UAllCards : public UDataAsset
{
	GENERATED_BODY()

	public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UUCardData*> allCards;
};
