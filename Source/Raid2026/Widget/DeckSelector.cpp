// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckSelector.h"

#include "Kismet/GameplayStatics.h"
#include "Raid2026/CoreLayer/Cards/UCardData.h"
#include "Raid2026/Save/DeckListSave.h"
#include "Raid2026/Save/DeckSave.h"

bool UDeckSelector::CanCreateDeck()
{

	if (GetAllDeckNames().Num() >= MaxNumberOfDeck)
	{
		return false;
	}

	return true;
}

bool UDeckSelector::CanDeleteDeck()
{
	if (GetAllDeckNames().Num() > 1) return true;
	
	return false;
}

void UDeckSelector::InitializeDeck()
{
	InitializeDeckBP();
}

TArray<FString> UDeckSelector::GetAllDeckNames()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("DeckList"), 0))
	{
		return {};
	}

	UDeckListSave* DeckList = Cast<UDeckListSave>(UGameplayStatics::LoadGameFromSlot(TEXT("DeckList"),0));

	if (!DeckList)
	{
		return {};
	}

	return DeckList->DeckNames;
}

void UDeckSelector::DeleteDeck(const FString& DeckName)
{
	if (UGameplayStatics::DoesSaveGameExist(DeckName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(DeckName,0);
	}

	if (!UGameplayStatics::DoesSaveGameExist(TEXT("DeckList"), 0))
	{
		return;
	}

	UDeckListSave* DeckList = Cast<UDeckListSave>(UGameplayStatics::LoadGameFromSlot(TEXT("DeckList"),0));

	if (!DeckList)
	{
		return;
	}

	DeckList->DeckNames.Remove(DeckName);

	UGameplayStatics::SaveGameToSlot(DeckList,TEXT("DeckList"),0);
}

TMap<int32, int32> UDeckSelector::ModifyDeck(const FString& DeckName)
{
	if (!UGameplayStatics::DoesSaveGameExist(DeckName, 0))
	{
		return {};
	}

	UDeckSave* Save = Cast<UDeckSave>(UGameplayStatics::LoadGameFromSlot(DeckName,0));

	if (!Save)
	{
		return {};
	}

	return Save->SavedDeck;
}

TArray<UUCardData*> UDeckSelector::TransformDeckNameToDeck(FString DeckName)
{
	TArray<TObjectPtr<UUCardData>> PlayerDeck;

	UDeckSave* Save = Cast<UDeckSave>(
		UGameplayStatics::LoadGameFromSlot(
			DeckName,
			0));

	for (const auto& Pair : Save->SavedDeck)
	{
		int32 CardId = Pair.Key;
		int32 Count = Pair.Value;

		UUCardData* FoundCard = nullptr;

		for (UUCardData* Card : AllCards->allCards)
		{
			if (Card && Card->cardId == CardId)
			{
				FoundCard = Card;
				break;
			}
		}

		if (!FoundCard)
		{
			continue;
		}

		for (int32 i = 0; i < Count; ++i)
		{
			PlayerDeck.Add(FoundCard);
		}
	}
	return PlayerDeck;
}