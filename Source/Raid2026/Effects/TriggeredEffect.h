#pragma once

#include "CoreMinimal.h"
#include "Effect.h"
#include "TriggeredEffect.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UTriggeredEffect : public UEffect
{
    GENERATED_BODY()

public:
    UTriggeredEffect()
    {
        EssenceCost = 0;
    }

    virtual bool CanApply_Implementation(const FEffectContext& Context) const override
    {
        return true;
    }
};
