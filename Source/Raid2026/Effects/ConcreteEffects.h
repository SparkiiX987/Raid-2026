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
        Description = FText::FromString(FString::Printf(TEXT("Quand ce vaisseau est révélé, piochez %d carte(s)."), DrawCount));
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
        Description = FText::FromString(FString::Printf(TEXT("À la fin de votre tour, gagnez + %d carburant bonus."), BonusAmount));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Essence",
        meta = (ClampMin = 1))
    int32 BonusAmount = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_TestOnStartTurn : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_TestOnStartTurn()
    {
        bCanCaptureRefinery = false;
        Trigger = EEffectTrigger::OnStartOfTurn;
        DisplayName = FText::FromString(TEXT("test effect"));
        Description = FText::FromString(FString::Printf(TEXT("test effect.")));
    }

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
        Description = FText::FromString(FString::Printf(TEXT("Quand ce vaisseau est détruit, inflige %d dégât(s) à tous les vaisseaux adjacents."), Damage));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Damage",
        meta = (ClampMin = 1))
    int32 Damage = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Damage")
    bool bAffectsAllies = false;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_ActivatedDrawOnStartTurn : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_ActivatedDrawOnStartTurn()
    {
        Trigger = EEffectTrigger::OnStartOfTurn;
        DisplayName = FText::FromString(FString::Printf(TEXT("Au début de votre tour, piocher %d carte"), DrawCount));
        Description = FText::FromString(FString::Printf(TEXT("Au début de votre tour, piocher %d carte"), DrawCount));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Draw",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DrawCount = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_WinResistanceWhenControl : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_WinResistanceWhenControl()
    {
        Trigger = EEffectTrigger::OnMoveShip;
        EffectMaxHealth = ResistanceToWin;
        DisplayName = FText::FromString(FString::Printf(TEXT("Si ce vaisseau controle la rafinerie, il gagne +%d de resistance"), ResistanceToWin));
        Description = FText::FromString(FString::Printf(TEXT("Si ce vaisseau controle la rafinerie, il gagne +%d de resistance"), ResistanceToWin));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|StatWin",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 ResistanceToWin = 2;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_GetResistanceFromInferiorShipClass : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_GetResistanceFromInferiorShipClass()
    {
        Trigger = EEffectTrigger::OnStartOfTurn;
        DisplayName = FText::FromString(FString::Printf(TEXT("Si un vaisseau de classe inferieur à %d est adjacent, Tornade-V model L gagne +%d de resistance"), InferiorClassShip,ResistanceToWin));
        Description = FText::FromString(FString::Printf(TEXT("Si un vaisseau de classe inferieur à %d est adjacent, Tornade-V model L gagne +%d de resistance"), InferiorClassShip,ResistanceToWin));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|InferiorClassShip",
        meta = (ClampMin = 0, ClampMax = 5))
    int32 InferiorClassShip = 3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|StatWin",
    meta = (ClampMin = 1, ClampMax = 3))
    int32 ResistanceToWin = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_GiveResistanceToInferiorShipClass : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_GiveResistanceToInferiorShipClass()
    {
        Trigger = EEffectTrigger::OnStartOfTurn;
        DisplayName = FText::FromString(FString::Printf(TEXT("Chaque vaisseau de classe inférieur a %d adjacent gagne +%d de resistance "), InferiorClassShip,ResistanceToGive));
        Description = FText::FromString(FString::Printf(TEXT("Chaque vaisseau de classe inférieur a %d adjacent gagne +%d de resistance "), InferiorClassShip,ResistanceToGive));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|InferiorClassShip",
        meta = (ClampMin = 0, ClampMax = 5))
    int32 InferiorClassShip = 3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|StatWin",
    meta = (ClampMin = 1, ClampMax = 3))
    int32 ResistanceToGive = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_OnShipBesideTakingDamage : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_OnShipBesideTakingDamage()
    {
        Trigger = EEffectTrigger::OnDamageTaken;
        DisplayName = FText::FromString(FString::Printf(TEXT("Si un vaisseau de classe inférieur à sa droite et/ou à sa gauche devais prendre des dégat, les %d premier dégat infligé à chaque tour ne sont pas apliquer"), DamageToReduce));
        Description = FText::FromString(FString::Printf(TEXT("Si un vaisseau de classe inférieur à sa droite et/ou à sa gauche devais prendre des dégat, les %d premier dégat infligé à chaque tour ne sont pas apliquer"), DamageToReduce));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Stats",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DamageToReduce = 2;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|InferiorClassShip",
    meta = (ClampMin = 0, ClampMax = 5))
    int32 InferiorClassShip = 4;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_ActivatedDrawOnPlayExpertCard : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_ActivatedDrawOnPlayExpertCard()
    {
        Trigger = EEffectTrigger::OnPlayCard;
        DisplayName = FText::FromString(FString::Printf(TEXT("Quand vous jouer une carte Expert piocher %d carte"), DrawCount));
        Description = FText::FromString(FString::Printf(TEXT("Quand vous jouer une carte Expert piocher %d carte"), DrawCount));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Draw",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DrawCount = 1;
    
    ECardType TypeOfCardToPlay = ECardType::EXPERT;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_ActivatedDrawOnPlaySuperiorShipClassCard : public UTriggeredEffect
{
    GENERATED_BODY()

public:
    UEffect_ActivatedDrawOnPlaySuperiorShipClassCard()
    {
        Trigger = EEffectTrigger::OnPlayCard;
        DisplayName = FText::FromString(FString::Printf(TEXT("A chaque fois que vous jouer un vaisseau de classe supérieur à %d, piocher %d carte "), SuperiorClassShip,DrawCount));
        Description = FText::FromString(FString::Printf(TEXT("A chaque fois que vous jouer un vaisseau de classe supérieur à %d, piocher %d carte "), SuperiorClassShip,DrawCount));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Draw",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DrawCount = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|SuperiorClassShip",
    meta = (ClampMin = 0, ClampMax = 5))
    int32 SuperiorClassShip = 2;
    
    ECardType TypeOfCardToPlay = ECardType::SHIP;

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
        Description = FText::FromString(FString::Printf(TEXT("Payez %d → Piochez %d carte(s)."), EssenceCost, DrawCount));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Draw",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DrawCount = 1;

    virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_ActivatedResuscitationOfCard : public UActivatedEffect
{
    GENERATED_BODY()

public:
    UEffect_ActivatedResuscitationOfCard()
    {
        Trigger = EEffectTrigger::Activated;
        EssenceCost = 5;
        DisplayName = FText::FromString(FString::Printf(TEXT("Payer %d essence : recupérer une carte vaisseau ou amelioration depuis la décharge"), EssenceCost));
        Description = FText::FromString(FString::Printf(TEXT("Payer %d essence : recupérer une carte vaisseau ou amelioration depuis la décharge"), EssenceCost));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Draw",
        meta = (ClampMin = 1, ClampMax = 3))
    int32 DrawCount = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|TypeOfCard")
    TArray<ECardType> TypeOfCardRecoverable = {ECardType::SHIP,ECardType::UPGRADE};

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
        Description = FText::FromString(FString::Printf(TEXT("Payez %d → Poussez un vaisseau ciblé d'une case."), EssenceCost));
    }

    virtual EEffectTargetKind GetTargetKind() const override { return EEffectTargetKind::Ship; }

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
        Description = FText::FromString(FString::Printf(TEXT("Ce vaisseau n'est détecté qu'à portée %d de radar ennemi."), StealthRange));
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

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_HyperSpace : public UPassiveEffect
{
    GENERATED_BODY()

public:
    UEffect_HyperSpace()
    {
        bCanMoveOnFirstTurn = true;
        Trigger = EEffectTrigger::Passive;
        DisplayName = FText::FromString(TEXT("Hyperespace"));
        Description = FText::FromString(FString::Printf(TEXT("Ce vaisseau peut se déplacer au premier tour.")));
    }

};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_CanCapture : public UPassiveEffect
{
    GENERATED_BODY()

public:
    UEffect_CanCapture()
    {
        bCanCaptureRefinery = true;
        Trigger = EEffectTrigger::Passive;
        DisplayName = FText::FromString(TEXT("Controleur"));
        Description = FText::FromString(FString::Printf(TEXT("Ce vaisseau peut contrôler les rafineries.")));
    }
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_BigCanon : public UPassiveEffect
{
    GENERATED_BODY()

public:
    UEffect_BigCanon()
    {
        bHaveBigCanon = true;
        Trigger = EEffectTrigger::Passive;
        DisplayName = FText::FromString(TEXT("BigCanon"));
        Description = FText::FromString(FString::Printf(TEXT("Peut tirer 2 fois par tour et effectuer un tir avec lui coute une essence suplémentaire")));
    }
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_DiagonalMovement : public UPassiveEffect
{
    GENERATED_BODY()

public:
    UEffect_DiagonalMovement()
    {
        bMoveInDiagonal = true;
        Trigger = EEffectTrigger::Passive;
        DisplayName = FText::FromString(TEXT("DiagonalMovement"));
        Description = FText::FromString(FString::Printf(TEXT("Ce vaisseau se déplace uniquement en diagonale")));
    }
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UEffect_CanSpawnShipBesideHim : public UPassiveEffect
{
    GENERATED_BODY()

public:
    UEffect_CanSpawnShipBesideHim()
    {
        bCanSpawnShipBesideHim = true;
        Trigger = EEffectTrigger::Passive;
        DisplayName = FText::FromString(TEXT("CanSpawnShipBesideHim"));
        Description = FText::FromString(FString::Printf(TEXT("Vous pouvez faire apparaitre les vaisseau de classe inférieur que vous jouer adjacent à lui ")));
    }
};