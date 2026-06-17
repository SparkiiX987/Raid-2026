#pragma once

#include "CoreMinimal.h"
#include "ShipEffectList.generated.h"

USTRUCT()
struct FShipEffectList
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<TObjectPtr<UEffect>> Effects;
};