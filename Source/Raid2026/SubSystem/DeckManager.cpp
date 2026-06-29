#include "DeckManager.h"
#include "EffectManager.h"

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
    }

    BroadcastHandUpdate(State);
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
        return false;
    }

    State.Hand.RemoveAt(Index);
    State.Discard.Add(card);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("DiscardCard P%d : '%s' défaussée (%d en main)"),
            playerId, *card->GetName(), State.Hand.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    OnCardDiscarded.Broadcast(playerId, card);
    BroadcastHandUpdate(State);

    return true;
}

bool UDeckManager::PlayCard(int32 playerId, UUCardData* card)
{
    if (!card) return false;

    FPlayerDeckState& State = GetOrCreateState(playerId);

    const int32 Index = State.Hand.Find(card);
    if (Index == INDEX_NONE)
    {
        return false;
    }

    State.Hand.RemoveAt(Index);
    OnCardPlayed.Broadcast(playerId, card);

    NotifyOnPlayCardEffects(playerId, card);
    BroadcastHandUpdate(State);
    return true;
}

void UDeckManager::NotifyOnPlayCardEffects(int32 PlayerId, UUCardData* CardPlay)
{
    if (!effectManager || !IsValid(CardPlay))
    {
        return;
    }

    FEffectContext Context;
    Context.OwnerPlayerID = PlayerId;
    Context.CardPlay = CardPlay;

    effectManager->NotifyEvent(EEffectTrigger::OnPlayCard, Context);
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
    Result.bDeckEmpty = state.Deck.Num() > 0;

    OnCardDrawn.Broadcast(state.playerId, card);

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
