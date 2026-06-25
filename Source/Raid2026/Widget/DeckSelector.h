#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Raid2026/CoreLayer/Cards/AllCards.h"
#include "Raid2026/CoreLayer/Cards/UCardData.h"
#include "DeckSelector.generated.h"

UCLASS()
class RAID2026_API UDeckSelector : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SelectedDeckName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxNumberOfDeck;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAllCards> AllCards;

	UFUNCTION(BlueprintPure)
	bool CanCreateDeck();

	UFUNCTION(BlueprintPure)
	bool CanDeleteDeck();

	UFUNCTION(BlueprintCallable)
	void InitializeDeck();

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeDeckBP();

	UFUNCTION(BlueprintPure)
	TArray<FString> GetAllDeckNames();
	
	UFUNCTION(BlueprintCallable)
	void DeleteDeck(const FString& DeckName);

	UFUNCTION(BlueprintCallable)
	TMap<int32, int32> ModifyDeck(const FString& DeckName);

	UFUNCTION(BlueprintCallable)
	TArray<UUCardData*> TransformDeckNameToDeck(FString DeckName);
};
