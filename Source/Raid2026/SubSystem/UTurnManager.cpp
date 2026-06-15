#include "UTurnManager.h"
#include "../CoreLayer/Turn/ETurnPhase.h"
#include "../Actor/AShip.h"
#include "UBoardManager.h"
#include <Raid2026/PlayerState/BoardPlayerState.h>
#include <Kismet/GameplayStatics.h>


void UUTurnManager::StartTurn(int32 playerId)
{
    activePlayerId = playerId;
    ++currentTurn;
    SetTurnPhase(ETurnPhase::StartTurn);

    if (GEngine)
    {
        FString text = FString::Printf(
            TEXT("=== Tour %d · Joueur P%d ==="), 
            currentTurn, 
            playerId
        );

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    //check condition de victoire

    RefillEssence(playerId);

    // Effets OnStartOfTurn

    if (boardManager)
    {
        for (int32 X = 0; X < UUBoardManager::GridWidth; ++X)
            for (int32 Y = 0; Y < UUBoardManager::GridHeight + 2; ++Y)
            {
                AAShip* Ship = boardManager->GetShipAt(FIntPoint(X, Y));
                if (IsValid(Ship) && Ship->GetOwnerID() == playerId)
                {
                    Ship->ResetTurnFlags();
                }
            }
    }

    SetTurnPhase(ETurnPhase::Main);
    OnTurnStarted.Broadcast(playerId, currentTurn);

    if (GEngine)
    {
        FString text = FString::Printf(
            TEXT("StartTurn P%d: Main — base=%d/%d · bonus=%d"),
            playerId,
            GetCurrentEssence(playerId), 
            GetMaxEssence(playerId),
            GetBonusEssence(playerId)
        );

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

void UUTurnManager::EndTurn()
{
    if (currentPhase != ETurnPhase::Main)
    {
        if (GEngine)
        {
            FString text = FString::Printf(
                TEXT("EndTurn: hors phase Main (phase=%s)"),
                *UEnum::GetValueAsString(currentPhase)
            );

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return;
    }

    const int32 EndingPlayer = activePlayerId;
    SetTurnPhase(ETurnPhase::EndTurn);

    // Effets OnEndOfTurn

    if (deckManager)
    {
        TArray<UUCardData*> Discarded = deckManager->EnforceHandLimit(EndingPlayer);
        if (Discarded.Num() > 0)
        {
            if (GEngine)
            {
                FString text = FString::Printf(
                    TEXT("EndTurn P%d: %d carte(s) défaussée(s)"),
                    EndingPlayer, Discarded.Num()
                );

                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
            }
        }
    }

    OnTurnEnded.Broadcast(EndingPlayer);
}

bool UUTurnManager::PayEssence(int32 playerId, int32 cost)
{
    if (cost <= 0) return true;

    FEssenceState& S = GetOrCreateEssenceState(playerId);

    if (S.GetTotalEssence() < cost)
    {
        if (GEngine)
        {
            FString text = FString::Printf(
                TEXT("PayEssence P%d: insolvable — coût=%d · dispo=%d"),
                playerId,
                cost,
                S.GetTotalEssence()
            );

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }

        return false;
    }

    int32 Rem = cost;

    if (S.bonusEssence > 0)
    {
        const int32 FromBonus = FMath::Min(S.bonusEssence, Rem);
        S.bonusEssence -= FromBonus;
        Rem -= FromBonus;
        OnEssenceSpent.Broadcast(playerId, FromBonus, true);
    }

    if (Rem > 0)
    {
        S.currentEssence -= Rem;
        OnEssenceSpent.Broadcast(playerId, Rem, false);
    }

    if (GEngine)
    {
        FString text = FString::Printf(
            TEXT("PayEssence P%d: -%d -> base=%d/%d · bonus=%d"),
            playerId,
            cost,
            S.currentEssence,
            S.maxEssence,
            S.bonusEssence
        );

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    return true;
}

void UUTurnManager::AddBonusEssence(int32 playerId, int32 amount)
{
    if (amount <= 0) return;

    FEssenceState& S = GetOrCreateEssenceState(playerId);
    S.bonusEssence += amount;

    OnBonusEssenceGained.Broadcast(playerId, amount);

    if (GEngine)
    {
        FString text = FString::Printf(
            TEXT("AddBonusEssence P%d: +%d → total bonus=%d"),
            playerId,
            amount,
            S.bonusEssence
        );

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

int32 UUTurnManager::GetAvaliableEssence(int32 playerId) const
{
    const FEssenceState* S = essenceStates.Find(playerId);
    return S ? S->GetTotalEssence() : 0;
}

int32 UUTurnManager::GetCurrentEssence(int32 playerId) const
{
    const FEssenceState* S = essenceStates.Find(playerId);
    return S ? S->currentEssence : 0;
}

int32 UUTurnManager::GetBonusEssence(int32 playerId) const
{
    const FEssenceState* S = essenceStates.Find(playerId);
    return S ? S->bonusEssence : 0;
}

int32 UUTurnManager::GetMaxEssence(int32 playerId) const
{
    const FEssenceState* S = essenceStates.Find(playerId);
    return S ? S->maxEssence : 0;
}

bool UUTurnManager::CanAfford(int32 playerId, int32 cost) const
{
	return GetMaxEssence(playerId) >= cost;
}

int32 UUTurnManager::GetCurrentTurn() const
{
	return currentTurn;
}

int32 UUTurnManager::GetCurrentPlayer() const
{
	return activePlayerId;
}

ETurnPhase UUTurnManager::GetCurrentTurnPhase() const
{
	return currentPhase;
}

bool UUTurnManager::IsPlayerTurn(int32 playerId) const
{
    return activePlayerId == playerId && currentPhase == ETurnPhase::Main;
}

void UUTurnManager::InitializeGame(int32 inFirstPlayerId, int32 inPlayerCount)
{
    playerCount = inPlayerCount;
    firstPlayerId = inFirstPlayerId;
    bIsFirstTurnOfGame = true;
    currentTurn = 0;

    StartTurn(firstPlayerId);
}

void UUTurnManager::SetTurnPhase(ETurnPhase newPhase)
{
    const ETurnPhase Old = currentPhase;
    currentPhase = newPhase;
    OnPhaseChanged.Broadcast(Old, newPhase);
}

void UUTurnManager::RefillEssence(int32 playerId)
{
    FEssenceState& S = GetOrCreateEssenceState(playerId);

    const bool bIsVeryFirstTurn = (currentTurn == 1 && playerId == firstPlayerId);
    if (!bIsVeryFirstTurn)
        S.maxEssence += essenceIncrement;

    S.currentEssence = S.maxEssence;

    if (GEngine)
    {
        FString text = FString::Printf(
            TEXT("RefillEssence P%d: base=%d/%d · bonus=%d (conservé)"),
            playerId,
            S.currentEssence,
            S.maxEssence,
            S.bonusEssence
        );

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

int32 UUTurnManager::GetNextPlayerId() const
{
    return (activePlayerId + 1) % playerCount;
}

FEssenceState& UUTurnManager::GetOrCreateEssenceState(int32 playerId)
{
    if (!essenceStates.Contains(playerId))
    {
        FEssenceState S;
        S.maxEssence = startEssence;
        S.currentEssence = startEssence;
        S.bonusEssence = 0;
        essenceStates.Add(playerId, S);
    }
    return essenceStates[playerId];
}
