// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../CoreLayer/Cells/FCell.h"
#include "ABoardCell.generated.h"

UCLASS()
class RAID2026_API AABoardCell : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCell cellData;
};