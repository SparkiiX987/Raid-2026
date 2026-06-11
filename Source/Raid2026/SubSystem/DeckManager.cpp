#include "DeckManager.h"

void UDeckManager::InitializeDeck(int32 playerId, const TArray<UUCardData*>& CardList)
{
        if (CardList.Num() != DeckSize)
        {
            if (GEngine)
            {
                FString text = FString::Printf(TEXT("InitializeDeck P%d : %d cartes (attendu %d)"),
                    playerId, CardList.Num(), DeckSize);

                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
            }
        }

    FPlayerDeckState& State = GetOrCreateState(playerId);
    State.Deck = CardList;
    State.Hand.Empty();
    State.Discard.Empty();

    ShuffleDeck(State.Deck);

    for (int32 i = 0; i < StartingHand && State.Deck.Num() > 0; ++i)
    {

        DrawSingleCard(State);
        /*UUCardData* Card = State.Deck[0];
        State.Deck.RemoveAt(0);
        State.Hand.Add(Card);

        OnCardDrawn.Broadcast(playerId, Card);*/
    }

    BroadcastHandUpdate(State);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("InitializeDeck P%d : %d en main, %d en deck"),
            playerId, State.Hand.Num(), State.Deck.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

TArray<FDrawResult> UDeckManager::DrawCards(int32 playerId, int32 drawCount)
{
    if (drawCount == -1)
    {
        drawCount = DrawPerTurn;
    }

    TArray<FDrawResult> Results;

    if (drawCount <= 0) return Results;

    FPlayerDeckState& State = GetOrCreateState(playerId);

    for (int32 i = 0; i < drawCount; ++i)
    {
        FDrawResult Result = DrawSingleCard(State);
        Results.Add(Result);

        if (Result.bDeckEmpty) break;
    }

    BroadcastHandUpdate(State);
    return Results;
}

bool UDeckManager::DiscardCard(int32 playerId, UUCardData* card)
{
    if (!card) return false;

    FPlayerDeckState& State = GetOrCreateState(playerId);

    const int32 Index = State.Hand.Find(card);
    if (Index == INDEX_NONE)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("DiscardCard P%d : carte '%s' non trouvée en main"),
                playerId, *card->GetName());

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return false;
    }

    State.Hand.RemoveAt(Index);
    State.Discard.Add(card);

    OnCardDiscarded.Broadcast(playerId, card);
    BroadcastHandUpdate(State);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("DiscardCard P%d : '%s' défaussée (%d en main)"),
            playerId, *card->GetName(), State.Hand.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    return true;
}

bool UDeckManager::PlayCard(int32 playerId, UUCardData* card)
{
    if (!card) return false;

    FPlayerDeckState& State = GetOrCreateState(playerId);

    const int32 Index = State.Hand.Find(card);
    if (Index == INDEX_NONE)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("PlayCard P%d : '%s' non trouvée en main"),
                playerId, *card->GetName());

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return false;
    }

    State.Hand.RemoveAt(Index);
    OnCardPlayed.Broadcast(playerId, card);

    if (card->type == ECardType::SABOTAGE)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("PlayCard P%d : sabotage '%s' → décharge immédiate"),
                playerId, *card->GetName());

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }

    }
    else
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("PlayCard P%d : '%s' jouée sur le plateau"),
                playerId, *card->GetName());

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
    }

    BroadcastHandUpdate(State);
    return true;
}

TArray<UUCardData*> UDeckManager::EnforceHandLimit(int32 playerId)
{
    TArray<UUCardData*> Discarded;

    FPlayerDeckState& State = GetOrCreateState(playerId);

    while (State.Hand.Num() > MaxHandSize)
    {
        UUCardData* ToDiscard = State.Hand.Last();
        State.Hand.Pop();
        State.Discard.Add(ToDiscard);
        Discarded.Add(ToDiscard);

        OnCardDiscarded.Broadcast(playerId, ToDiscard);

        if (GEngine)
        {
            FString text = FString::Printf(TEXT("EnforceHandLimit P%d : '%s' défaussée (main = %d/%d)"),
                playerId, *ToDiscard->GetName(),
                State.Hand.Num(), MaxHandSize);

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
    }

    if (Discarded.Num() > 0)
    {
        BroadcastHandUpdate(State);
    }

    return Discarded;
}

void UDeckManager::SendPlayedCardToDiscard(int32 playerId, UUCardData* card)
{
    if (!card) return;

    FPlayerDeckState& State = GetOrCreateState(playerId);
    State.Discard.Add(card);

    OnCardDiscarded.Broadcast(playerId, card);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("SendPlayedCardToDiscard P%d : '%s' → décharge"),
            playerId, *card->GetName());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

const TArray<UUCardData*>& UDeckManager::GetHand(int32 playerId) const
{
    static const TArray<UUCardData*> Empty;
    const FPlayerDeckState* State = PlayerDecks.Find(playerId);
    return State ? State->Hand : Empty;
}

const TArray<UUCardData*>& UDeckManager::GetDiscard(int32 playerId) const
{
    static const TArray<UUCardData*> Empty;
    const FPlayerDeckState* State = PlayerDecks.Find(playerId);
    return State ? State->Discard : Empty;
}

int32 UDeckManager::GetDeckCount(int32 playerId) const
{
    const FPlayerDeckState* State = PlayerDecks.Find(playerId);
    return State ? State->Deck.Num() : 0;
}

int32 UDeckManager::GetHandCount(int32 playerId) const
{
    const FPlayerDeckState* State = PlayerDecks.Find(playerId);
    return State ? State->Hand.Num() : 0;
}

bool UDeckManager::IsCardInHand(int32 playerId, UUCardData* card) const
{
    const FPlayerDeckState* State = PlayerDecks.Find(playerId);
    return State && card && State->Hand.Contains(card);
}

FPlayerDeckState UDeckManager::GetDeckState(int32 playerId) const
{
    const FPlayerDeckState* State = PlayerDecks.Find(playerId);
    return State ? *State : FPlayerDeckState{};
}

void UDeckManager::ShuffleDeck(TArray<UUCardData*>& deck)
{
    const int32 N = deck.Num();
    for (int32 i = N - 1; i > 0; --i)
    {
        const int32 j = FMath::RandRange(0, i);
        deck.Swap(i, j);
    }
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ShuffleDeck: %d cartes mélangées"), N);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

FDrawResult UDeckManager::DrawSingleCard(FPlayerDeckState& state)
{
    FDrawResult Result;

    if (state.Deck.Num() == 0)
    {
        Result.bDeckEmpty = true;
        Result.DamageTaken = EmptyDeckDmg;
        Result.DrawnCard = nullptr;

        OnEmptyDeckDamage.Broadcast(state.playerId, EmptyDeckDmg);

        if (GEngine)
        {
            FString text = FString::Printf(TEXT("DrawSingleCard P%d : deck vide → %d dégât(s)"),
                state.playerId, EmptyDeckDmg);

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, text);
        }
        return Result;
    }

    UUCardData* card = state.Deck[0];
    state.Deck.RemoveAt(0);
    state.Hand.Add(card);

    Result.DrawnCard = card;
    Result.bDeckEmpty = false;

    OnCardDrawn.Broadcast(state.playerId, card);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("DrawSingleCard P%d : '%s' piochée (%d restantes)"),
            state.playerId, *card->GetName(), state.Deck.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, text);
    }

    return Result;
}

FPlayerDeckState& UDeckManager::GetOrCreateState(int32 playerId)
{
    if (!PlayerDecks.Contains(playerId))
    {
        FPlayerDeckState NewState;
        NewState.playerId = playerId;
        PlayerDecks.Add(playerId, NewState);
    }

    return PlayerDecks[playerId];
}

void UDeckManager::BroadcastHandUpdate(const FPlayerDeckState& State)
{
    OnHandUpdated.Broadcast(State.playerId, State.Hand);
}
