#pragma once

#include "CoreMinimal.h"
#include "Effect.h"
#include "ActivatedEffect.h"
#include "TriggeredEffect.h"
#include "PassiveEffect.h"
#include "../CoreLayer/Cells/EDirection.h"
#include "../CoreLayer/Cards/CardStats.h"
#include "ConcreteEffects.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_DrawOnReveal : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_DrawOnReveal()
    {
        Trigger = EEffectTrigger::OnReveal;
        DisplayName = FText::FromString(TEXT("Prise d'information"));
        Description = FText::FromString(TEXT("Quand ce vaisseau est révélé, piochez {DrawCount} carte(s)."));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Draw",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DrawCount = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_BonusEssenceOnTurn : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_BonusEssenceOnTurn()
    {
        Trigger = EEffectTrigger::OnEndOfTurn;
        DisplayName = FText::FromString(TEXT("Extraction"));
        Description = FText::FromString(TEXT("À la fin de votre tour, gagnez +{BonusAmount} carburant bonus."));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Essence",
        meta = (ClampMin = 1))
    int32 BonusAmount = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_DamageOnDestroyed : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_DamageOnDestroyed()
    {
        Trigger = EEffectTrigger::OnDestroyed;
        DisplayName = FText::FromString(TEXT("Explosion finale"));
        Description = FText::FromString(TEXT("Quand ce vaisseau est détruit, inflige {Damage} dégât(s) à tous les vaisseaux adjacents."));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Damage",
        meta = (ClampMin = 1))
    int32 Damage = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Damage")
    bool bAffectsAllies = false;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_ActivatedDraw : public UActivatedEffect
{
    GENERATED_BODY()

public:
    UEffect_ActivatedDraw()
    {
        Trigger = EEffectTrigger::Activated;
        EssenceCost = 2;
        DisplayName = FText::FromString(TEXT("Analyse de données"));
        Description = FText::FromString(TEXT("Payez {EssenceCost} → Piochez {DrawCount} carte(s)."));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Draw",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DrawCount = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_ActivatedPush : public UActivatedEffect
{
    GENERATED_BODY()

public:
    UEffect_ActivatedPush()
    {
        Trigger = EEffectTrigger::Activated;
        EssenceCost = 1;
        DisplayName = FText::FromString(TEXT("Impulsion gravitationnelle"));
        Description = FText::FromString(TEXT("Payez {EssenceCost} → Poussez un vaisseau ciblé d'une case."));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Push",
        meta = (ClampMin = 1))
    int32 Range = 2;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Push")
    bool bFixedDirection = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Push",
        meta = (EditCondition = "bFixedDirection"))
    EDirections FixedDirection = EDirections::North;

    virtual bool CanApply_Implementation(const FEffectContext& Context) const override;
    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};


UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_StatBoost : public UPassiveEffect
{
    GENERATED_BODY()

public:
    UEffect_StatBoost()
    {
        Trigger = EEffectTrigger::Passive;
        DisplayName = FText::FromString(TEXT("Amélioration système"));
        Description = FText::FromString(TEXT("Modifie les statistiques du vaisseau porteur."));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Stats")
    FCardStats StatBonus;

    virtual FCardStats GetPassiveStatBonus_Implementation() const override
    {
        return StatBonus;
    }
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_Stealth : public UPassiveEffect
{
    GENERATED_BODY()

public:
    UEffect_Stealth()
    {
        Trigger = EEffectTrigger::Passive;
        DisplayName = FText::FromString(TEXT("Furtivité avancée"));
        Description = FText::FromString(TEXT("Ce vaisseau n'est détecté qu'à portée {StealthRange} de radar ennemi."));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Stealth",
        meta = (ClampMin = 0))
    int32 StealthRange = 1;

    virtual FCardStats GetPassiveStatBonus_Implementation() const override
    {
        FCardStats Bonus;
        Bonus.radar = -StealthRange;
        return Bonus;
    }
};