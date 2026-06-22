#pragma once

#include "CoreMinimal.h"
#include "Sabotage.h"
#include "ConcreteSabotage.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UIEMAttack : public USabotage
{
	GENERATED_BODY()

public:
    UIEMAttack()
    {
        Trigger = EEffectTrigger::Activated;
        DisplayName = FText::FromString(TEXT("Attaque IEM"));
        Description = FText::FromString(FString::Printf(TEXT("Quand appliquer sur un vaisseau ennemie, ce dernier ne peux pas jouer pendant 1 tour.")));
    }

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UImplementedSabotage : public USabotage
{
    GENERATED_BODY()

public:
    UImplementedSabotage()
    {
        Trigger = EEffectTrigger::Activated;
        DisplayName = FText::FromString(TEXT("Sabotage"));
        Description = FText::FromString(FString::Printf(TEXT("Détruit une carte expert ou amélioration en jeu.")));
    }

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEmperorGift : public USabotage
{
    GENERATED_BODY()

public:
    UEmperorGift()
    {
        Trigger = EEffectTrigger::Activated;
        DisplayName = FText::FromString(TEXT("Cadeau de l'empereur"));
        Description = FText::FromString(FString::Printf(TEXT("Piochez une carte et l'adversaire se défause d'une carte.")));
    }

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEssenceExplosion : public USabotage
{
    GENERATED_BODY()

public:
    UEssenceExplosion()
    {
        Trigger = EEffectTrigger::Activated;
        DisplayName = FText::FromString(TEXT("Explosion d'essence"));
        Description = FText::FromString(FString::Printf(TEXT("Réduit à 0 la reserve d'essence bonus de l'adversaire")));
    }

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};