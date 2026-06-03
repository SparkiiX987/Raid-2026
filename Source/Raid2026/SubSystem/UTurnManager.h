#pragma once

#include "CoreMinimal.h"
#include "../CoreLayer/Essence/FEssenceState.h"
#include "UTurnManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UUTurnManager : public UObject
{
	GENERATED_BODY()
	
public:
	static constexpr int32 startEssence = 5;
	static constexpr int32 essenceIncrement = 1;
	static constexpr int32 fireCost = 1;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTurnStarted, int32, PlayerID, int32, TurnNumber);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnEnded, int32, PlayerID);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseChanged, ETurnPhase, OldPhase, ETurnPhase, NewPhase);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEssenceSpent, int32, PlayerID, int32, Amount, bool, bWasBonus);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBonusEssenceGained, int32, PlayerID, int32, Amount);

    UPROPERTY(BlueprintAssignable, Category = "Events")
        FOnTurnStarted OnTurnStarted;

    UPROPERTY(BlueprintAssignable, Category = "Events") 
        FOnTurnEnded OnTurnEnded;

    UPROPERTY(BlueprintAssignable, Category = "Events") 
        FOnPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events") 
        FOnEssenceSpent OnEssenceSpent;

    UPROPERTY(BlueprintAssignable, Category = "Events") 
        FOnBonusEssenceGained OnBonusEssenceGained;

    UFUNCTION(BlueprintCallable)
        void StartTurn(int32 playerId);

    UFUNCTION(BlueprintCallable)
        void EndTurn();

    UFUNCTION(BlueprintCallable)
        bool PayEssence(int32 playerId, int32 cost);

    UFUNCTION(BlueprintCallable)
        void AddBonusEssence(int32 playerId, int32 amount);

    UFUNCTION(BlueprintPure)
        int32 GetAvaliableEssence(int32 playerId) const;

    UFUNCTION(BlueprintPure)
        int32 GetCurrentEssence(int32 playerId) const;

    UFUNCTION(BlueprintPure)
        int32 GetBonusEssence(int32 playerId) const;

    UFUNCTION(BlueprintPure)
        int32 GetMaxEssence(int32 playerId) const;

    UFUNCTION(BlueprintPure)
        bool CanAfford(int32 playerId, int32 cost) const;

    UFUNCTION(BlueprintPure)
        int32 GetCurrentTurn() const;

    UFUNCTION(BlueprintPure)
        int32 GetCurrentPlayer() const;

    UFUNCTION(BlueprintPure)
        ETurnPhase GetCurrentTurnPhase() const;

    UFUNCTION(BlueprintPure)
        bool IsPlayerTurn(int32 playerId) const;

    UFUNCTION(BlueprintCallable)
        void InitializeGame(int32 inFirstPlayerId, int32 inPlayerCount = 2);

private:
    UPROPERTY()
        TMap<int32, FEssenceState> essenceStates;

    UPROPERTY()
        int32 currentTurn;

    UPROPERTY()
        int32 activePlayerId;

    UPROPERTY()
        int32 playerCount;

    UPROPERTY()
        ETurnPhase currentPhase;

    UPROPERTY()
        bool bIsFirstTurnOfGame;

    UPROPERTY()
        int32 firstPlayerId;

    void SetTurnPhase(ETurnPhase newPhase);

    void RefillEssence(int32 playerId);

    int32 GetNextPlayerId() const;

    FEssenceState& GetOrCreateEssenceState(int32 playerId);
};
