#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../CoreLayer/Effects/EEffectTrigger.h"
#include "../CoreLayer/Effects/FEffectResult.h"
#include "../CoreLayer/Effects/FEffectContext.h"
#include "../CoreLayer/Cards/CardStats.h"
#include "Effect.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UEffect : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true))
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EEffectTrigger Trigger = EEffectTrigger::Activated;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, EditCondition = "Trigger==EEffectTrigger::Activated"))
    int32 EssenceCost = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "Trigger==EEffectTrigger::Passive"))
        bool canCaptureRefinery;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    FEffectResult Apply(const FEffectContext& Context);
    virtual FEffectResult Apply_Implementation(const FEffectContext& Context);

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    bool CanApply(const FEffectContext& Context) const;
    virtual bool CanApply_Implementation(const FEffectContext& Context) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    FCardStats GetPassiveStatBonus() const;
    virtual FCardStats GetPassiveStatBonus_Implementation() const;

    UFUNCTION(BlueprintPure)
    bool MatchesTrigger(EEffectTrigger InTrigger) const { return Trigger == InTrigger; }

    virtual FString GetDebugName() const
    {
        return FString::Printf(TEXT("[%s]"), *DisplayName.ToString());
    }
};
