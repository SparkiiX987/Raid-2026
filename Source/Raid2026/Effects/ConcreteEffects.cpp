
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
    /*if (!Context.Board || !Context.SourceShip.IsValid())
        return FEffectResult::Fail(TEXT("DamageOnDestroyed: Board ou Source manquant"));

    UGameInstance* GI = Context.Board->GetGameInstance();
    UCombatResolver* Combat = GI ? GI->GetSubsystem<UCombatResolver>() : nullptr;
    if (!Combat)
        return FEffectResult::Fail(TEXT("DamageOnDestroyed: CombatResolver introuvable"));

    const FIntPoint Origin = Context.SourceShip->GetGridPosition();
    TArray<FIntPoint> Adjacent = GetAdjacentCells(Origin);
    */
    FEffectResult Out = FEffectResult::Success();
    /*
    for (const FIntPoint& Cell : Adjacent)
    {
        AShipActor* Neighbor = Context.Board->GetShipAt(Cell);
        if (!Neighbor) continue;

        const bool bIsAlly = (Neighbor->GetOwnerID() == Context.OwnerPlayerID);
        if (bIsAlly && !bAffectsAllies) continue;

        Combat->ApplyDamageToShip(Neighbor, Damage);
        Out.AffectedCells.Add(Cell);

        if (GEngine)
        {
            FString text = FString::Printf(TEXT("Effect_DamageOnDestroyed: %s → %d dégât(s) sur %s"),
                *Context.SourceShip->GetName(), Damage, *Neighbor->GetName());

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
    }

    Out.IntValue = Damage;*/
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
    if (!Super::CanApply_Implementation(Context)) return false;

    if (!Context.TargetShip.IsValid()) return false;

    if (!Context.SourceShip.IsValid()) return false;

    const FIntPoint Delta =
        Context.TargetShip->GetGridPosition() - Context.SourceShip->GetGridPosition();
    const int32 ManhattanDist = FMath::Abs(Delta.X) + FMath::Abs(Delta.Y);

    return ManhattanDist <= Range;
}

FEffectResult UEffect_ActivatedPush::Apply_Implementation(const FEffectContext& Context)
{
    /*if (!Context.TargetShip.IsValid())
        return FEffectResult::NeedsTarget();

    if (!Context.Turn)
        return FEffectResult::Fail(TEXT("ActivatedPush: TurnManager absent"));

    const bool bPaid = Context.Turn->PayEssence(Context.OwnerPlayerID, EssenceCost);
    if (!bPaid)
        return FEffectResult::Fail(TEXT("ActivatedPush: Paiement échoué"));

    UGameInstance* GI = GetGameInstance<UGameInstance>();
    UCombatResolver* Combat = GI ? GI->GetSubsystem<UCombatResolver>() : nullptr;
    if (!Combat)
        return FEffectResult::Fail(TEXT("ActivatedPush: CombatResolver introuvable"));

    EDirections Dir = FixedDirection;
    if (!bFixedDirection)
    {
        const FIntPoint Delta =
            Context.TargetShip->GetGridPosition() - Context.SourceShip->GetGridPosition();
        Dir = IntPointToDirection(Delta);
    }

    FPushResult PushResult = Combat->ApplyPush(Context.TargetShip.Get(), Dir);*/
    
    FEffectResult Out = FEffectResult::Success();
    /*Out.bNeedsTarget = !PushResult.bMoved && PushResult.Collisions.Num() == 0;
    Out.IntValue = PushResult.Collisions.Num();
    Out.AffectedCells.Add(PushResult.FinalCell);
    
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Effect_ActivatedPush: %s poussé → (%d,%d), %d collision(s)"),
            *Context.TargetShip->GetName(),
            PushResult.FinalCell.X, PushResult.FinalCell.Y,
            PushResult.Collisions.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }*/

    return Out;
}