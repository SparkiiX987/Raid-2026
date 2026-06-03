#include "UTurnManager.h"
#include "../CoreLayer/Turn/ETurnPhase.h"

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

    const bool bSkipDraw = bIsFirstTurnOfGame && (playerId == firstPlayerId);
    if (!bSkipDraw)
    {
        /*if (UDeckManager* Deck = GetDeck())
            Deck->DrawCards(playerId, 1);*/
    }
    else
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("StartTurn P%d: pioche ignorée (premier tour)"), playerId);

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        bIsFirstTurnOfGame = false;
    }

    // Effets OnStartOfTurn

    // ResetShipFlags pour tous les vaisseaux du joueur actif
    /*if (UBoardManager* Board = GetBoard())
    {
        for (int32 X = 0; X < UBoardManager::GridWidth; ++X)
            for (int32 Y = 0; Y < UBoardManager::GridHeight; ++Y)
            {
                AShipActor* Ship = Board->GetShipAt(FIntPoint(X, Y));
                if (Ship && Ship->GetOwnerID() == PlayerID)
                    Ship->ResetTurnFlags();
            }
    }*/

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

    /*if (UDeckManager* Deck = GetDeck())
    {
        TArray<UCardData*> Discarded = Deck->EnforceHandLimit(EndingPlayer);
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
    }*/

    OnTurnEnded.Broadcast(EndingPlayer);

    // 3. Tour suivant
    StartTurn(GetNextPlayerId());
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
	return essenceStates[playerId].maxEssence;
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

    for (int32 i = 0; i < playerCount; ++i)
    {
        FEssenceState& S = GetOrCreateEssenceState(i);
        S.maxEssence = startEssence;
        S.currentEssence = startEssence;
        S.bonusEssence = 0;
    }

    if (GEngine)
    {
        FString text = FString::Printf(
            TEXT("UTurnManager: InitializeGame — %d joueurs, premier=P%d"),
            playerCount, 
            firstPlayerId
        );

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

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

/*UBoardManager* UTurnManager::GetBoard()   const { auto* GI = GetGameInstance(); return GI ? GI->GetSubsystem<UBoardManager>() : nullptr; }
UDeckManager* UTurnManager::GetDeck()    const { auto* GI = GetGameInstance(); return GI ? GI->GetSubsystem<UDeckManager>() : nullptr; }
UEffectManager* UTurnManager::GetEffects() const { auto* GI = GetGameInstance(); return GI ? GI->GetSubsystem<UEffectManager>() : nullptr; }*/
