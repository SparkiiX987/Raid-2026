
#include "ConcreteEffects.h"
#include "../SubSystem/DeckManager.h"
#include "../SubSystem/UTurnManager.h"
#include "../SubSystem/UBoardManager.h"
#include "../SubSystem/CombatResolver.h"
#include "../Actor/AShip.h"

namespace
{
    TArray<FIntPoint> GetAdjacentCells(FIntPoint Center)
    {
        return {
            Center + FIntPoint(0,  1),
            Center + FIntPoint(0, -1),
            Center + FIntPoint(-1,  0),
            Center + FIntPoint(1,  0),
        };
    }
}

FEffectResult UEffect_DrawOnReveal::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Deck)
        return FEffectResult::Fail(TEXT("DrawOnReveal: DeckManager absent du contexte"));

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Effect_DrawOnReveal: Joueur %d pioche %d carte(s)"),
        Context.OwnerPlayerID, DrawCount);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    TArray<FDrawResult> Results = Context.Deck->DrawCards(Context.OwnerPlayerID, DrawCount);
    
    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = Results.Num();

    for (const FDrawResult& R : Results)
    {
        if (R.DrawnCard)
            Out.AffectedCards.Add(R.DrawnCard);
    }
    
    return Out;
}

FEffectResult UEffect_BonusEssenceOnTurn::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn)
        return FEffectResult::Fail(TEXT("BonusEssenceOnTurn: TurnManager absent"));

    Context.Turn->AddBonusEssence(Context.OwnerPlayerID, BonusAmount);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Effect_BonusEssenceOnTurn: Joueur %d +%d essence bonus"),
        Context.OwnerPlayerID, BonusAmount);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }
    
    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = BonusAmount;
    return Out;
}

FEffectResult UEffect_DamageOnDestroyed::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Board || !Context.SourceShip.IsValid())
        return FEffectResult::Fail(TEXT("DamageOnDestroyed: Board ou Source manquant"));

    const FIntPoint Origin = Context.SourceShip->GetGridPosition();
    TArray<FIntPoint> Adjacent = GetAdjacentCells(Origin);
    
    FEffectResult Out = FEffectResult::Success();
    for (const FIntPoint& Cell : Adjacent)
    {
        AAShip* Neighbor = Context.Board->GetShipAt(Cell);
        if (!Neighbor) continue;

        const bool bIsAlly = (Neighbor->GetOwnerID() == Context.OwnerPlayerID);
        if (bIsAlly && !bAffectsAllies) continue;

        Context.Resolver.Get()->ApplyDamageToShip(Neighbor, Damage);
        Out.AffectedCells.Add(Cell);

        if (GEngine)
        {
            FString text = FString::Printf(TEXT("Effect_DamageOnDestroyed: %s → %d dégât(s) sur %s"),
                *Context.SourceShip->GetName(), Damage, *Neighbor->GetName());

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
    }

    Out.IntValue = Damage;
    return Out;
}

FEffectResult UEffect_ActivatedDrawOnStartTurn::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn || !Context.Deck)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    TArray<FDrawResult> Results = Context.Deck->DrawCards(Context.OwnerPlayerID, DrawCount);
    
    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = Results.Num();
    for (const FDrawResult& R : Results)
        if (R.DrawnCard) Out.AffectedCards.Add(R.DrawnCard);

    return Out;
}

FEffectResult UEffect_WinResistanceWhenControl::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn || !Context.Deck)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    if (Context.Board->Grid[Context.SourceShip->gridPosition.X][Context.SourceShip->gridPosition.Y].refinery == nullptr)
    {
        return FEffectResult::Fail(TEXT("Pas sur une raffinerie"));
    }
    Context.SourceShip->Heal(2);
    
    
    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = ResistanceToWin;

    return Out;
}

FEffectResult UEffect_GetResistanceFromInferiorShipClass::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn || !Context.Deck)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    for (const FIntPoint& Direction : Context.Directions)
    {
        const FIntPoint NewCell = Context.SourceShip->gridPosition + Direction;

        if (AAShip* Ship = Cast<AAShip>(Context.Board->GetCell(NewCell).Occupant))
        {
            if (Ship->GetOwnerID() == Context.OwnerPlayerID && Ship->CardData->shipClass < InferiorClassShip)
            {
                Context.SourceShip->Heal(ResistanceToWin);
                return FEffectResult::Success();
            }
        }
    }
    
    return FEffectResult::Fail(TEXT("Pas de vaisseau de classe inferieur a 3 a proximité"));
}

FEffectResult UEffect_GiveResistanceToInferiorShipClass::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn || !Context.Deck)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    TArray<AAShip*> InferiorClassShipClose;

    for (const FIntPoint& Direction : Context.Directions)
    {
        const FIntPoint NewCell = Context.SourceShip->gridPosition + Direction;

        if (AAShip* Ship = Cast<AAShip>(Context.Board->GetCell(NewCell).Occupant))
        {
            if (Ship->GetOwnerID() == Context.OwnerPlayerID && Ship->CardData->shipClass < InferiorClassShip)
            {
                InferiorClassShipClose.Add(Ship);
            }
        }
    }

    if (InferiorClassShipClose.Num() <= 0)
    {
        return FEffectResult::Fail(TEXT("Pas de vaisseau de classe inferieur a 3 a proximité"));
    }

    for (int i = 0; i < InferiorClassShipClose.Num(); i++)
    {
        InferiorClassShipClose[i]->Heal(ResistanceToGive);
    }
    
    return FEffectResult::Success();
}

FEffectResult UEffect_OnShipBesideTakingDamage::Apply_Implementation(const FEffectContext& Context)
{
    if (Context.TargetShip == nullptr || !Context.Turn)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    if (AAShip* Ship = Cast<AAShip>(Context.TargetShip))
    {
        
        if (Ship->GetOwnerID() != Context.OwnerPlayerID
            || Ship->CardData->shipClass >= InferiorClassShip
            || Ship->gridPosition != Context.SourceShip->gridPosition + FIntPoint(1,0) && Ship->gridPosition != Context.SourceShip->gridPosition - FIntPoint(1,0))
            return FEffectResult::Fail(TEXT("Pas de vaisseau a proteger"));

        if (Context.DamageDealt >= DamageToReduce)
        {
            Ship->protection += DamageToReduce;
        }
        else
        {
            Ship->protection += Context.DamageDealt;
        }
        FEffectResult Out = FEffectResult::Success();
        Out.IntValue = Ship->CardData->stats.resistance;
        return Out;
    }
    return FEffectResult::Fail(TEXT("Pas de vaisseau trouver"));
}

FEffectResult UEffect_ActivatedDrawOnPlayExpertCard::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn || !Context.Deck)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    if (Context.CardPlay->type != TypeOfCardToPlay)
    {
        return FEffectResult::Fail(TEXT("Mauvaise carte"));
    }

    TArray<FDrawResult> Results = Context.Deck->DrawCards(Context.OwnerPlayerID, DrawCount);

    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = Results.Num();
    for (const FDrawResult& R : Results)
        if (R.DrawnCard) Out.AffectedCards.Add(R.DrawnCard);

    return Out;
    
}

FEffectResult UEffect_ActivatedDrawOnPlaySuperiorShipClassCard::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn || !Context.Deck)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    if (Context.CardPlay->type != TypeOfCardToPlay)
    {
        return FEffectResult::Fail(TEXT("Mauvaise carte"));
    }

    if (Context.CardPlay->shipClass <= SuperiorClassShip)
    {
        return FEffectResult::Fail(TEXT("Vaisseau trop petit"));
    }

    TArray<FDrawResult> Results = Context.Deck->DrawCards(Context.OwnerPlayerID, DrawCount);

    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = Results.Num();
    for (const FDrawResult& R : Results)
        if (R.DrawnCard) Out.AffectedCards.Add(R.DrawnCard);

    return Out;
    
}

FEffectResult UEffect_ActivatedDraw::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.Turn || !Context.Deck)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Subsystems manquants"));

    const bool bPaid = Context.Turn->PayEssence(Context.OwnerPlayerID, EssenceCost);
    if (!bPaid)
        return FEffectResult::Fail(TEXT("ActivatedDraw: Paiement échoué"));

    TArray<FDrawResult> Results = Context.Deck->DrawCards(Context.OwnerPlayerID, DrawCount);
    
    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = Results.Num();
    for (const FDrawResult& R : Results)
        if (R.DrawnCard) Out.AffectedCards.Add(R.DrawnCard);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Effect_ActivatedDraw: Joueur %d paye %d → pioche %d"),
            Context.OwnerPlayerID, EssenceCost, DrawCount);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    return Out;
}

bool UEffect_ActivatedPush::CanApply_Implementation(const FEffectContext& Context) const
{
    return Super::CanApply_Implementation(Context);
}

FEffectResult UEffect_ActivatedPush::Apply_Implementation(const FEffectContext& Context)
{
    if (!Context.SourceShip.IsValid())
        return FEffectResult::Fail(TEXT("ActivatedPush: source invalide"));

    if (!Context.TargetShip.IsValid())
        return FEffectResult::NeedsTarget();

    if (!Context.Turn || !Context.Resolver)
        return FEffectResult::Fail(TEXT("ActivatedPush: subsystems manquants"));

    const FIntPoint Delta = Context.TargetShip->GetGridPosition() - Context.SourceShip->GetGridPosition();
    if (FMath::Abs(Delta.X) + FMath::Abs(Delta.Y) > Range)
        return FEffectResult::Fail(TEXT("ActivatedPush: cible hors de portee"));

    if (!Context.Turn->PayEssence(Context.OwnerPlayerID, EssenceCost))
        return FEffectResult::Fail(TEXT("ActivatedPush: paiement echoue"));

    AAShip* Target = Cast<AAShip>(Context.TargetShip.Get());
    if (!Target) return FEffectResult::Fail(TEXT("ActivatedPush: cible invalide"));

    EDirections Dir = bFixedDirection ? FixedDirection : Context.Resolver->IntPointToDirection(Delta);
    const FPushResult Push = Context.Resolver->ApplyPush(Target, Dir);

    FEffectResult Out = FEffectResult::Success();
    Out.IntValue = Push.Collisions.Num();
    Out.AffectedCells.Add(Push.FinalCell);
    return Out;
}

FEffectResult UEffect_TestOnStartTurn::Apply_Implementation(const FEffectContext& Context)
{
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("test effect on start turn"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    return FEffectResult::Success();
}
