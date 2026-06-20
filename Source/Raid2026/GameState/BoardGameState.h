#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "../CoreLayer/Turn/ETurnPhase.h"
#include "BoardGameState.generated.h"

UCLASS()
class RAID2026_API ABoardGameState : public AGameState
{
	GENERATED_BODY()
	
public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnActivePlayerChanged, int32, NewActivePlayerID);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnGridStateChanged, const TArray<FReplicatedCellState>&, NewGrid);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnGameOver, int32, WinnerPlayerID);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnTurnPhaseChanged, ETurnPhase, CurrentPhase);

    virtual void BeginPlay() override;

    UPROPERTY(BlueprintAssignable)
        FOnActivePlayerChanged OnActivePlayerChanged;

    UPROPERTY(BlueprintAssignable)
        FOnGridStateChanged OnGridStateChanged;

    UPROPERTY(BlueprintAssignable)
        FOnGameOver OnGameOver;

    UPROPERTY(BlueprintAssignable)  
    FOnTurnPhaseChanged OnTurnPhaseChanged;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ActivePlayer)
        int32 ActivePlayerID = -1;

    UPROPERTY(BlueprintReadOnly, Replicated)
        int32 TurnNumber = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TurnPhase)
        ETurnPhase CurrentPhase;

    UPROPERTY(ReplicatedUsing = OnRep_GridState)
        TArray<FReplicatedCellState> GridState;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WinnerID)
        int32 WinnerId = -1;

    UPROPERTY(Replicated, BlueprintReadOnly)
    float CurrentTurnTimer;

    UPROPERTY(Replicated, BlueprintReadOnly)
    float Player1Timer;

    UPROPERTY(Replicated, BlueprintReadOnly)
    float Player2Timer;

    void SetActivePlayer(int32 PlayerID, int32 Turn, ETurnPhase Phase);
    void SetTurnPhase(ETurnPhase phase);
    void UpdateCellState(FIntPoint pos, const FReplicatedCellState & newState);
    void SetWinner(int32 winnerPlayerId);

    UFUNCTION()
        void OnRep_ActivePlayer();

    UFUNCTION()
        void OnRep_GridState();

    UFUNCTION()
        void OnRep_TurnPhase();

    UFUNCTION()
        void OnRep_WinnerID();

    int32 CellIndex(int32 X, int32 Y) const;

    FReplicatedCellState GetCellState(FIntPoint Pos) const;
};
