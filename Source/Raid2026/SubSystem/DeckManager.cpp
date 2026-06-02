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
        UUCardData* Card = State.Deck[0];
        State.Deck.RemoveAt(0);
        State.Hand.Add(Card);

        OnCardDrawn.Broadcast(playerId, Card);
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
	return TArray<FDrawResult>();
}

bool UDeckManager::DiscardCard(int32 playerId, UUCardData* card)
{
	return false;
}

bool UDeckManager::PlayCard(int32 playerId, UUCardData* card)
{
	return false;
}

TArray<UUCardData*> UDeckManager::EnforceHandLimit(int32 playerId)
{
	return TArray<UUCardData*>();
}

const TArray<UUCardData*>& UDeckManager::GetHand(int32 playerId) const
{
	// TODO: insérer une instruction return ici
}

const TArray<UUCardData*>& UDeckManager::GetDiscard(int32 playerId) const
{
	// TODO: insérer une instruction return ici
}

int32 UDeckManager::GetDeckCount(int32 playerId) const
{
	return int32();
}

int32 UDeckManager::GetHandCount(int32 playerId) const
{
	return int32();
}

bool UDeckManager::IsCardInHand(int32 playerId, UUCardData* card) const
{
	return false;
}

FPlayerDeckState UDeckManager::GetDeckState(int32 playerId) const
{
	return FPlayerDeckState();
}

void UDeckManager::Initialize(FSubsystemCollectionBase& collection)
{
    Super::Initialize(collection);
    PlayerDecks.Empty();
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("UDeckManager: Initialisé"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

void UDeckManager::Deinitialize()
{
    PlayerDecks.Empty();
    Super::Deinitialize();
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

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
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

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
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
