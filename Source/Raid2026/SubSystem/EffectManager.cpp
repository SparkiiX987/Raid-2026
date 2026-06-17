#include "EffectManager.h"
#include "UBoardManager.h"


void UEffectManager::Initialize(UUBoardManager* BoardManager, UUTurnManager* TurnManager, UDeckManager* Deckmanager)
{
    boardManager = BoardManager;
    turnManager = TurnManager;
    deckManager = Deckmanager;
}

FEffectContext UEffectManager::BuildContextWithSubsystems(FEffectContext BaseContext) const
{
    if (!BaseContext.Board) BaseContext.Board = boardManager;
    if (!BaseContext.Turn)  BaseContext.Turn = turnManager;
    if (!BaseContext.Deck)  BaseContext.Deck = deckManager;

    return BaseContext;
}

void UEffectManager::RegisterShipEffects(AAShip* Ship,
    const TArray<UEffect*>& Effects)
{
    if (!Ship || Effects.IsEmpty()) return;

    TArray<TObjectPtr<UEffect>>& Existing = RegisteredEffects.FindOrAdd(Ship);

    for (UEffect* Effect : Effects)
    {
        if (Effect && !Existing.Contains(Effect))
        {
            Existing.Add(Effect);
        }
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("EffectManager: %s → %d effet(s) enregistré(s)"),
            *Ship->GetName(), Existing.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }
}

void UEffectManager::UnregisterShipEffects(AAShip* Ship)
{
    if (!Ship) return;

    const int32 Removed = RegisteredEffects.Remove(Ship);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("EffectManager: %s → %d entrée(s) supprimée(s)"),
            *Ship->GetName(), Removed);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }
}

void UEffectManager::NotifyEvent(EEffectTrigger Trigger,
    const FEffectContext& BaseContext)
{
    if (RegisteredEffects.IsEmpty()) return;

    const FEffectContext Context = BuildContextWithSubsystems(BaseContext);

    TArray<TObjectPtr<AAShip>> ShipSnapshot;
    RegisteredEffects.GetKeys(ShipSnapshot);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("EffectManager::NotifyEvent — trigger=%s · %d ships"),
            *UEnum::GetValueAsString(Trigger), ShipSnapshot.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    for (AAShip* Ship : ShipSnapshot)
    {
        if (!IsValid(Ship)) continue;

        const bool bTurnScopedTrigger =
            (Trigger == EEffectTrigger::OnStartOfTurn ||
                Trigger == EEffectTrigger::OnEndOfTurn);

        if (bTurnScopedTrigger && Context.OwnerPlayerID != -1)
        {
            if (Ship->GetOwnerID() != Context.OwnerPlayerID)
                continue;
        }

        TArray<TObjectPtr<UEffect>>* EffectList = RegisteredEffects.Find(Ship);
        if (!EffectList) continue;

        TArray<UEffect*> EffectSnapshot = *EffectList;

        for (UEffect* Effect : EffectSnapshot)
        {
            if (!IsValid(Effect)) continue;

            if (!Effect->MatchesTrigger(Trigger)) continue;

            FEffectContext EffectCtx = Context;
            EffectCtx.SourceShip = Ship;
            EffectCtx.OwnerPlayerID = Ship->GetOwnerID();

            if (!Effect->CanApply(EffectCtx))
            {
                if (GEngine)
                {
                    FString text = FString::Printf(TEXT("  [%s] sur %s → CanApply() = false"),
                        *Effect->GetDebugName(), *Ship->GetName());

                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
                }
                continue;
            }

            if (GEngine)
            {
                FString text = FString::Printf(TEXT("  [%s] sur %s → Apply()"),
                    *Effect->GetDebugName(), *Ship->GetName());

                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
            }

            const FEffectResult Result = Effect->Apply(EffectCtx);

            OnEffectApplied.Broadcast(Effect, Result);

            if (!Result.bSuccess && !Result.bNeedsTarget)
            {
                if (GEngine)
                {
                    FString text = FString::Printf(TEXT("  [%s] échec : %s"),
                        *Effect->GetDebugName(), *Result.FailReason);

                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
                }
            }
        }
    }
}

FEffectResult UEffectManager::ActivateEffect(UEffect* Effect,
    const FEffectContext& BaseContext)
{
    if (!IsValid(Effect))
        return FEffectResult::Fail(TEXT("Effet invalide"));

    FEffectContext Context = BuildContextWithSubsystems(BaseContext);

    if (Effect->Trigger != EEffectTrigger::Activated)
    {
        return FEffectResult::Fail(
            FString::Printf(TEXT("%s n'est pas un effet activé"),
                *Effect->GetDebugName()));
    }

    if (!Effect->CanApply(Context))
    {
        return FEffectResult::Fail(
            FString::Printf(TEXT("%s : CanApply() = false (essence insuffisante ?)"),
                *Effect->GetDebugName()));
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("EffectManager::ActivateEffect — [%s] par P%d"),
            *Effect->GetDebugName(), Context.OwnerPlayerID);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    const FEffectResult Result = Effect->Apply(Context);

    if (Result.bSuccess)
        OnEffectApplied.Broadcast(Effect, Result);

    return Result;
}

TArray<UEffect*> UEffectManager::GetAvailableActivatedEffects(
    AAShip* Ship,
    const FEffectContext& BaseContext) const
{
    TArray<UEffect*> Available;
    if (!IsValid(Ship)) return Available;

    const TArray<TObjectPtr<UEffect>>* EffectList = RegisteredEffects.Find(Ship);
    if (!EffectList) return Available;

    const FEffectContext Context = BuildContextWithSubsystems(BaseContext);

    for (UEffect* Effect : *EffectList)
    {
        if (!IsValid(Effect))               continue;
        if (Effect->Trigger != EEffectTrigger::Activated) continue;

        FEffectContext EffectCtx = Context;
        EffectCtx.SourceShip = Ship;
        EffectCtx.OwnerPlayerID = Ship->GetOwnerID();

        if (Effect->CanApply(EffectCtx))
            Available.Add(Effect);
    }

    return Available;
}

bool UEffectManager::CanCaptureRefinery(AAShip* Ship)
{
    for (UEffect* effect : RegisteredEffects[Ship])
    {
        if (!effect->canCaptureRefinery)
        {
            return false;
        }
    }

    return true;
}
