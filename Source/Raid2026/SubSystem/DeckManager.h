#pragma once

#include "CoreMinimal.h"
#include "../CoreLayer/Cards/DrawResult.h"
#include "../CoreLayer/Cards/PlayerDeckState.h"
#include "DeckManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UDeckManager : public UObject
{
	GENERATED_BODY()
	
public:
	static constexpr int32 DeckSize = 30;
	static constexpr int32 StartingHand = 6;
	static constexpr int32 MaxHandSize = 6;
	static constexpr int32 DrawPerTurn = 1;
	static constexpr int32 EmptyDeckDmg = 1;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCardDrawn, int32, PlayerID, UUCardData*, Card);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCardDiscarded, int32, PlayerID, UUCardData*, Card);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCardPlayed, int32, PlayerID, UUCardData*, Card);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHandUpdated, int32, PlayerID, const TArray<UUCardData*>&, NewHand);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEmptyDeckDamage, int32, PlayerID, int32, Damage);

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnCardDrawn OnCardDrawn;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnCardDiscarded OnCardDiscarded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnCardPlayed OnCardPlayed;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHandUpdated OnHandUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnEmptyDeckDamage OnEmptyDeckDamage;

	UFUNCTION(BlueprintCallable)
		void InitializeDeck(int32 playerId, const TArray<UUCardData*>& CardList);

	UFUNCTION(BlueprintCallable)
		TArray<FDrawResult> DrawCards(int32 playerId, int32 drawCount = -1);

	UFUNCTION(BlueprintCallable)
		bool DiscardCard(int32 playerId, UUCardData* card);

	UFUNCTION(BlueprintCallable)
		bool PlayCard(int32 playerId, UUCardData* card);

	UFUNCTION(BlueprintCallable)
		TArray<UUCardData*> EnforceHandLimit(int32 playerId);

	UFUNCTION(BlueprintPure)
		const TArray<UUCardData*>& GetHand(int32 playerId) const;


	UFUNCTION(BlueprintPure)
		const TArray<UUCardData*>& GetDiscard(int32 playerId) const;

	UFUNCTION(BlueprintPure)
		int32 GetDeckCount(int32 playerId) const;

	UFUNCTION(BlueprintPure)
		int32 GetHandCount(int32 playerId) const;

	UFUNCTION(BlueprintPure)
		bool IsCardInHand(int32 playerId, UUCardData* card) const;

	UFUNCTION(BlueprintPure)
		FPlayerDeckState GetDeckState(int32 playerId) const;

	void SendPlayedCardToDiscard(int32 playerId, UUCardData* card);

private:

	UPROPERTY()
		TMap<int32, FPlayerDeckState> PlayerDecks;

	void ShuffleDeck(TArray<UUCardData*>& deck);

	FDrawResult DrawSingleCard(FPlayerDeckState& state);

	FPlayerDeckState& GetOrCreateState(int32 playerId);

	void BroadcastHandUpdate(const FPlayerDeckState& state);
};
