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
        void RegisterShipEffects(AAShip* Ship, const TArray<UEffect*>& Effects);

    UFUNCTION(BlueprintCallable)
        void UnregisterShipEffects(AAShip* Ship);

    UFUNCTION(BlueprintCallable)
        void NotifyEvent(EEffectTrigger Trigger, const FEffectContext& Context);

    UFUNCTION(BlueprintCallable)
        FEffectResult ActivateEffect(UEffect* Effect, const FEffectContext& Context);

    UFUNCTION(BlueprintCallable)
        TArray<UEffect*> GetAvailableActivatedEffects(AAShip* Ship, const FEffectContext& Context) const;

    UFUNCTION(BlueprintCallable)
        TArray<UEffect*> GetEffectsOfPlayer(EEffectTrigger effectTrigger, int32 playerId) const;

    UFUNCTION(BlueprintCallable)
        bool CanCaptureRefinery(AAShip* Ship);

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
        TMap<TObjectPtr<AAShip>, FShipEffectList> RegisteredEffects;

};
