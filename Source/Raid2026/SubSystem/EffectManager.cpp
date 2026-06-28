#include "EffectManager.h"
#include "UBoardManager.h"
#include "CombatResolver.h"


void UEffectManager::Initialize(UUBoardManager* BoardManager, UUTurnManager* TurnManager, UDeckManager* Deckmanager, UCombatResolver* CombatResolver)
{
    boardManager = BoardManager;
    turnManager = TurnManager;
    deckManager = Deckmanager;
    combatResolver = CombatResolver;

    combatResolver->OnShipDestroyed.AddDynamic(this, &UEffectManager::UnregisterShipEffects);
}

FEffectContext UEffectManager::BuildContextWithSubsystems(FEffectContext BaseContext) const
{
    if (!BaseContext.Board) BaseContext.Board = boardManager;
    if (!BaseContext.Turn)  BaseContext.Turn = turnManager;
    if (!BaseContext.Deck)  BaseContext.Deck = deckManager;
    if (!BaseContext.Resolver)  BaseContext.Resolver = combatResolver;

    return BaseContext;
}

void UEffectManager::RegisterEffects(AABoardActor* Ship,
    const TArray<UEffect*>& Effects)
{
    if (!IsValid(Ship) || Effects.IsEmpty()) return;

    FShipEffectList& Existing = RegisteredEffects.FindOrAdd(Ship);

    for (UEffect* Effect : Effects)
    {
        if (Effect && !Existing.Effects.Contains(Effect))
        {
            Existing.Effects.Add(Effect);
        }
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("EffectManager: %s → %d effet(s) enregistré(s)"),
            *Ship->GetName(), Existing.Effects.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }
}

void UEffectManager::UnregisterShipEffects(AAShip* Ship)
{
    NotifyShipEvent(Ship, EEffectTrigger::OnDestroyed, FEffectContext());
    UnregisterEffects(Ship);
}

void UEffectManager::UnregisterEffects(AABoardActor* Ship)
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

    TArray<TObjectPtr<AABoardActor>> ShipSnapshot;
    RegisteredEffects.GetKeys(ShipSnapshot);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("EffectManager::NotifyEvent — trigger=%s · %d ships"),
            *UEnum::GetValueAsString(Trigger), ShipSnapshot.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    for (AABoardActor* Ship : ShipSnapshot)
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

        FShipEffectList* EffectList = RegisteredEffects.Find(Ship);
        if (!EffectList) continue;

        for (UEffect* Effect : EffectList->Effects)
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

void UEffectManager::NotifyShipEvent(AABoardActor* Ship, EEffectTrigger Trigger, const FEffectContext& BaseContext)
{
    if (!IsValid(Ship)) return;

    FShipEffectList* List = RegisteredEffects.Find(Ship);
    if (!List) return;

    FEffectContext Context = BuildContextWithSubsystems(BaseContext);
    Context.SourceShip = Ship;
    Context.OwnerPlayerID = Ship->GetOwnerID();

    for (UEffect* Effect : List->Effects)
    {
        if (!IsValid(Effect) || !Effect->MatchesTrigger(Trigger)) continue;
        if (!Effect->CanApply(Context)) continue;

        const FEffectResult Result = Effect->Apply(Context);
        OnEffectApplied.Broadcast(Effect, Result);
    }
}

TArray<UEffect*> UEffectManager::GetActivatableEffects(AABoardActor* Ship, int32 PlayerId) const
{
    TArray<UEffect*> Out;
    const FShipEffectList* List = RegisteredEffects.Find(Ship);
    if (!List) return Out;

    const int32 Essence = turnManager ? turnManager->GetAvaliableEssence(PlayerId) : 0;
    for (UEffect* E : List->Effects)
    {
        if (IsValid(E) && E->Trigger == EEffectTrigger::Activated && E->EssenceCost <= Essence)
            Out.Add(E);
    }
    return Out;
}

TArray<UEffect*> UEffectManager::GetAvailableActivatedEffects(
    AABoardActor* Ship,
    const FEffectContext& BaseContext) const
{
    TArray<UEffect*> Available;
    if (!IsValid(Ship)) return Available;

    const FShipEffectList* EffectList = RegisteredEffects.Find(Ship);
    if (!EffectList) return Available;

    const FEffectContext Context = BuildContextWithSubsystems(BaseContext);

    for (UEffect* Effect : EffectList->Effects)
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

TArray<UEffect*> UEffectManager::GetEffectsOfPlayer(EEffectTrigger effectTrigger, int32 playerId) const
{
    TArray<UEffect*> effects = TArray<UEffect*>();

    TArray<TObjectPtr<AABoardActor>> ShipSnapshot;
    RegisteredEffects.GetKeys(ShipSnapshot);

    if (ShipSnapshot.Num() < 1)
        return effects;

    for (AABoardActor* ship : ShipSnapshot)
    {
        if (!IsValid(ship) || ship->ownerPlayer != playerId) continue;

        for (UEffect* effect : RegisteredEffects[ship].Effects)
        {
            if (IsValid(effect) && effect->Trigger == effectTrigger)
            {
                effects.Add(effect);
            }
        }
    }

    return effects;
}

bool UEffectManager::CanCaptureRefinery(AAShip* Ship)
{
    const FShipEffectList* List = RegisteredEffects.Find(Ship);

    if (!List) return false;

    for (const TObjectPtr<UEffect>& effect : List->Effects)
    {
        if (IsValid(effect) && effect->bCanCaptureRefinery)
            return true;
    }
        
    return false;
}

bool UEffectManager::HasHyperspace(AAShip* Ship)
{
    const FShipEffectList* List = RegisteredEffects.Find(Ship);

    if (!List) return false;

    for (const TObjectPtr<UEffect>& effect : List->Effects)
    {
        if (IsValid(effect) && effect->bCanMoveOnFirstTurn)
            return true;
    }

    return false;
}

bool UEffectManager::HaveBigCanon(AAShip* Ship)
{
    const FShipEffectList* List = RegisteredEffects.Find(Ship);

    if (!List) return false;

    for (const TObjectPtr<UEffect>& effect : List->Effects)
    {
        if (IsValid(effect) && effect->bHaveBigCanon)
            return true;
    }

    return false;
}

bool UEffectManager::MoveInDiagonale(AAShip* Ship)
{
    const FShipEffectList* List = RegisteredEffects.Find(Ship);

    if (!List) return false;

    for (const TObjectPtr<UEffect>& effect : List->Effects)
    {
        if (IsValid(effect) && effect->bMoveInDiagonal)
            return true;
    }

    return false;
}

bool UEffectManager::HasHyperspacePilote(AAMotherShip* Mothership)
{
    const FShipEffectList* List = RegisteredEffects.Find(Mothership);

    if (!List) return false;

    for (const TObjectPtr<UEffect>& effect : List->Effects)
    {
        if (IsValid(effect) && effect->bCanMoveOnFirstTurn)
            return true;
    }

    return false;
}

void UEffectManager::AddEffectsIfAbsent(AABoardActor* Ship, const TArray<UEffect*>& DesiredEffects)
{
    if (!IsValid(Ship)) return;

    FShipEffectList& Existing = RegisteredEffects.FindOrAdd(Ship);

    for (UEffect* Effect : DesiredEffects)
    {
        if (!IsValid(Effect)) continue;

        const bool bAlreadyRegistered = Existing.Effects.ContainsByPredicate(
            [Effect](const TObjectPtr<UEffect>& E)
            {
                return IsValid(E) && E->EffectID == Effect->EffectID;
            });

        if (!bAlreadyRegistered)
            Existing.Effects.Add(Effect);
    }
}

void UEffectManager::RemoveEffectsNotIn(AABoardActor* Ship, const TArray<UEffect*>& DesiredEffects)
{
    if (!IsValid(Ship)) return;

    FShipEffectList* List = RegisteredEffects.Find(Ship);
    if (!List) return;

    List->Effects.RemoveAll([&DesiredEffects](const TObjectPtr<UEffect>& Existing)
        {
            if (!IsValid(Existing)) return true;

            return !DesiredEffects.ContainsByPredicate(
                [&Existing](const UEffect* Desired)
                {
                    return IsValid(Desired) && Desired->GetClass() == Existing->GetClass();
                });
        });
}
