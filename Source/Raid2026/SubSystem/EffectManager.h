#pragma once

#include "CoreMinimal.h"
#include "../Actor/AShip.h"
#include "../Effects/Effect.h"
#include "../CoreLayer/Effects/ShipEffectList.h"
#include "EffectManager.generated.h"

class UUBoardManager;
class UUTurnManager;
class UDeckManager;

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UEffectManager : public UObject
{
	GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnEffectApplied, UEffect*, Effect, FEffectResult, Result);

public:
    UPROPERTY(BlueprintAssignable)
        FOnEffectApplied OnEffectApplied;

    UFUNCTION(BlueprintCallable)
        void Initialize(UUBoardManager* BoardManager, UUTurnManager* TurnManager, UDeckManager* Deckmanager, UCombatResolver* CombatResolver);

    UFUNCTION(BlueprintCallable)
        FEffectContext BuildContextWithSubsystems(FEffectContext BaseContext) const;

    UFUNCTION(BlueprintCallable)
        void RegisterEffects(AABoardActor* Ship, const TArray<UEffect*>& Effects);

    UFUNCTION(BlueprintCallable)
        void UnregisterShipEffects(AAShip* Ship);

    UFUNCTION(BlueprintCallable)
        void UnregisterEffects(AABoardActor* Ship);

    UFUNCTION(BlueprintCallable)
        void NotifyEvent(EEffectTrigger Trigger, const FEffectContext& Context);

    UFUNCTION(BlueprintCallable)
        FEffectResult ActivateEffect(UEffect* Effect, const FEffectContext& Context);

    void NotifyShipEvent(AABoardActor* Ship, EEffectTrigger Trigger, const FEffectContext& Context);

    TArray<UEffect*> GetActivatableEffects(AABoardActor* Ship, int32 PlayerId) const;

    UFUNCTION(BlueprintCallable)
        TArray<UEffect*> GetAvailableActivatedEffects(AABoardActor* Ship, const FEffectContext& Context) const;

    UFUNCTION(BlueprintCallable)
        TArray<UEffect*> GetEffectsOfPlayer(EEffectTrigger effectTrigger, int32 playerId) const;

    UFUNCTION(BlueprintCallable)
        bool CanCaptureRefinery(AAShip* Ship);

    UFUNCTION(BlueprintCallable)
        bool HasHyperspace(AAShip* Ship);

    UFUNCTION(BlueprintCallable)
        bool HasHyperspacePilote(AAMotherShip* Mothership);

    void AddEffectsIfAbsent(AABoardActor* Ship, const TArray<UEffect*>& DesiredEffects);

    void RemoveEffectsNotIn(AABoardActor* Ship, const TArray<UEffect*>& DesiredEffects);

    UPROPERTY(BlueprintReadWrite)
        TObjectPtr<UUBoardManager> boardManager;

    UPROPERTY(BlueprintReadWrite)
        TObjectPtr<UUTurnManager> turnManager;

    UPROPERTY(BlueprintReadWrite)
        TObjectPtr<UDeckManager> deckManager;

    UPROPERTY(BlueprintReadWrite)
        TObjectPtr<UCombatResolver> combatResolver;

protected:
    UPROPERTY()
        TMap<TObjectPtr<AABoardActor>, FShipEffectList> RegisteredEffects;

};
