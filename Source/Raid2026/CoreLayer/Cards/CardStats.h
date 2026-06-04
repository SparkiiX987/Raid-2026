#pragma once

#include "CoreMinimal.h"
#include "CardStats.generated.h"

USTRUCT(BlueprintType)
struct FCardStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 firePower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 resistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 radar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 moveCost;
};
