#pragma once

#include "CoreMinimal.h"
#include "FEssenceState.generated.h"

USTRUCT(BlueprintType)
struct FEssenceState
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 currentEssence;

	UPROPERTY(BlueprintReadOnly)
	int32 maxEssence;
	
	UPROPERTY(BlueprintReadOnly)
	int32 bonusEssence;
	
	int32 GetTotalEssence() const;
};
