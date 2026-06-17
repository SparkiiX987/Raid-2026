#pragma once

#include "CoreMinimal.h"
#include "Effect.h"
#include "PassiveEffect.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UPassiveEffect : public UEffect
{
    GENERATED_BODY()

public:
    UPassiveEffect()
    {
        Trigger = EEffectTrigger::Passive;
        EssenceCost = 0;
    }

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override
    {
        return FEffectResult::Fail(TEXT("Passif : utiliser GetPassiveStatBonus()"));
    }

    virtual bool CanApply_Implementation(const FEffectContext& Context) const override
    {
        return false;
    }
};