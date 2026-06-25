#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../CoreLayer/Cards/UCardData.h"
#include "Raid2026/CoreLayer/Cards/AllCards.h"
#include "BoardPlayerState.generated.h"

UCLASS()
class RAID2026_API ABoardPlayerState : public APlayerState
{
	GENERATED_BODY()
	
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnEssenceChanged, int32, Current, int32, Max);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnHandChanged, const TArray<UUCardData*>&, NewHand);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnHandCountChanged, int32, NewCount);

public:
    UPROPERTY(BlueprintAssignable)
        FOnEssenceChanged OnEssenceChanged;

    UPROPERTY(BlueprintAssignable)
        FOnHandChanged OnHandChanged;

    UPROPERTY(BlueprintAssignable)
        FOnHandCountChanged OnHandCountChanged;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Essence)
        int32 CurrentEssence;

    UPROPERTY(BlueprintReadOnly, Replicated)
        int32 MaxEssence;

    UPROPERTY(BlueprintReadOnly, Replicated)
        int32 BonusEssence;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HandCount)
        int32 HandCount;

    UPROPERTY(BlueprintReadOnly, Replicated)
        int32 DeckCount;

    UPROPERTY(BlueprintReadOnly, Replicated)
        int32 id;
    
    UPROPERTY(BlueprintReadOnly, Replicated)
     TArray<TObjectPtr<UUCardData>> deck;

    TMap<int32, TArray<UUCardData*>> allDecks;


    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Hand)
        TArray<UUCardData*> Hand;

    void SetEssence(int32 current, int32 max, int32 bonus);
    void SetHand(const TArray<UUCardData*>& newHand);
    void SetPlayerId(int32 id);

    UFUNCTION(BlueprintCallable)
    void AddDeck(TArray<UUCardData*> name);

    UFUNCTION(BlueprintImplementableEvent)
    void InitializeDeckBP();

    void UpdateEssenceUi();

    UFUNCTION()
        void OnRep_Hand();

    UFUNCTION()
        void OnRep_Essence();

    UFUNCTION()
        void OnRep_HandCount();
};
