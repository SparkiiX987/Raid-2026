#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Raid2026/CoreLayer/Cards/AllCards.h"
#include "Raid2026/CoreLayer/Cards/UCardData.h"
#include "DeckBuilder.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UDeckBuilder : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAllCards* AllCards;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int32, int32> NumberOfTheSameCards;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfCardsMax;

	UFUNCTION(BlueprintCallable)
	void AddACardToDeck(UUCardData* CardToAdd);

	UFUNCTION(BlueprintCallable)
	void RemoveACardFromDeck(UUCardData* CardToRemove);

	UFUNCTION(BlueprintCallable)
	void SaveDeck(const FString& DeckName);

	UFUNCTION(BlueprintImplementableEvent)
	void SaveDeckBP();

	UFUNCTION(BlueprintCallable)
	bool CanSaveDeck(const FString& DeckName);

	UFUNCTION(BlueprintCallable)
	void InitializeSetDeck(TMap<int32, int32> CardInDeck, FString DeckName);

	UFUNCTION(BlueprintPure)
	int32 GetTotalNumberOfCards() const;
};
