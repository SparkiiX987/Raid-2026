#include "DeckBuilder.h"
#include "Kismet/GameplayStatics.h"
#include "Raid2026/Save/DeckListSave.h"
#include "Raid2026/Save/DeckSave.h"

void UDeckBuilder::AddACardToDeck(UUCardData* CardToAdd)
{

	if (GetTotalNumberOfCards() >= NumberOfCardsMax)
	{
		return;
	}
	
	int NumberOfCards = NumberOfTheSameCards.Contains(CardToAdd->cardId) ? NumberOfTheSameCards[CardToAdd->cardId] +1 : 1;

	if (NumberOfCards > CardToAdd->maxNumber) return;
	
	NumberOfTheSameCards.FindOrAdd(CardToAdd->cardId) = NumberOfCards;
	
}

void UDeckBuilder::RemoveACardFromDeck(UUCardData* CardToRemove)
{
	int NumberOfCards = NumberOfTheSameCards.Contains(CardToRemove->cardId) ? NumberOfTheSameCards[CardToRemove->cardId] -1 : 0;
	
	if (NumberOfCards < 0) return;
	
	NumberOfTheSameCards.FindOrAdd(CardToRemove->cardId) = NumberOfCards;
}

bool UDeckBuilder::CanSaveDeck(const FString& DeckName)
{
	if (GetTotalNumberOfCards() < NumberOfCardsMax || DeckName.IsEmpty())
	{
		return false;
	}
	if (UGameplayStatics::DoesSaveGameExist(DeckName, 0) && DeckName != Name)
	{
		return false;
	}
	FString TrimmedName = DeckName.TrimStartAndEnd();

	if (TrimmedName != DeckName)
	{
		return false;
	}

	return true;
}

void UDeckBuilder::SaveDeck(const FString& DeckName)
{
	if (!CanSaveDeck(DeckName)) return;
	
	UDeckListSave* DeckList = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(TEXT("DeckList"), 0))
	{
		DeckList = Cast<UDeckListSave>(UGameplayStatics::LoadGameFromSlot(TEXT("DeckList"),0));
	}
	else
	{
		DeckList = Cast<UDeckListSave>(UGameplayStatics::CreateSaveGameObject(UDeckListSave::StaticClass()));
	}

	if (!DeckList)
	{
		return;
	}
	
	if (UGameplayStatics::DoesSaveGameExist(Name, 0))
	{
		UGameplayStatics::DeleteGameInSlot(Name,0);
	}
	DeckList->DeckNames.Remove(Name);

	UGameplayStatics::SaveGameToSlot(DeckList,TEXT("DeckList"),0);
	

	if (!DeckList->DeckNames.Contains(DeckName))
	{
		DeckList->DeckNames.Add(DeckName);
	}

	UGameplayStatics::SaveGameToSlot(DeckList,TEXT("DeckList"),0);

	UDeckSave* Save = Cast<UDeckSave>(UGameplayStatics::CreateSaveGameObject(UDeckSave::StaticClass()));

	if (!Save)
	{
		return;
	}
	
	Save->SavedDeck = NumberOfTheSameCards;

	UGameplayStatics::SaveGameToSlot(Save,DeckName,0);

	SaveDeckBP();
}

void UDeckBuilder::InitializeSetDeck(TMap<int32, int32> CardInDeck, FString DeckName)
{
	if (CardInDeck.IsEmpty()) return;

	NumberOfTheSameCards = CardInDeck;
	Name = DeckName;
}

int32 UDeckBuilder::GetTotalNumberOfCards() const
{
	int32 Total = 0;

	for (const auto& Pair : NumberOfTheSameCards)
	{
		Total += Pair.Value;
	}

	return Total;
}




