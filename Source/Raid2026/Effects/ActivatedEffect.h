#pragma once

#include "CoreMinimal.h"
#include "Effect.h"
#include "ActivatedEffect.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UActivatedEffect : public UEffect
{
    GENERATED_BODY()

public:
    UActivatedEffect()
    {
        Trigger = EEffectTrigger::Activated;
    }

    bool CanApply_Implementation(const FEffectContext& Context) const
    {
        if (!Context.Turn) return false;
        return Context.Turn->GetAvaliableEssence(Context.OwnerPlayerID) >= EssenceCost;
    }
};
