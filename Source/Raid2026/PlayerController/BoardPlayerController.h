#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Actor/ABoardCell.h"
#include "../Actor/ABoardVisualiser.h"
#include "../Actor/AMotherShip.h"
#include "../Actor/AShip.h"
#include "../CoreLayer/Inputs/ActionIntent.h"
#include <EnhancedInputSubsystems.h>
#include "../CoreLayer/Effects/FEffectContext.h"
#include "../CoreLayer/Effects/ActivableInfo.h"
#include "../CoreLayer/Effects/EffectTargetKind.h"
#include "BoardPlayerController.generated.h"

UCLASS()
class RAID2026_API ABoardPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

    void SetupPlayer(int32 ID);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    void ClearSelection();

#pragma region Request

    UFUNCTION(BlueprintCallable)
    void RequestEndTurn();

    UFUNCTION(BlueprintCallable)
    void RequestPlayCard(UUCardData* Card, FIntPoint TargetCell);

#pragma endregion

#pragma region Handle

    void HandleShipSelected(AAShip* Ship);
    void HandleCellTargeted(AABoardCell* Cell);

#pragma endregion

#pragma region Click

    UFUNCTION(BlueprintCallable)
        void ClickOnShip(AAShip* Ship);

    UFUNCTION(BlueprintCallable)
        void ClickOnCell(AABoardCell* Cell);

    UFUNCTION(BlueprintCallable)
        void ClickOnMotherShip(AAMotherShip* Mothership);

    UFUNCTION(BlueprintCallable)
        void OnResolveSabotageTarget(UUCardData* SelectedCard, int32 CardIndex);

#pragma endregion

#pragma region OnRep

    UFUNCTION()
        void OnRep_BoardVisualiser();

    UFUNCTION()
        void OnRep_PlayerID();

#pragma endregion

#pragma region ServerMethodes

        UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
            void ServerSetupFinish();
            bool ServerSetupFinish_Validate();

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerMoveShip(AAShip* Ship, FIntPoint TargetCell);
            bool ServerMoveShip_Validate(AAShip* Ship, FIntPoint TargetCell);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerFireAt(AAShip* Shooter, FIntPoint TargetCell);
            bool ServerFireAt_Validate(AAShip* Shooter, FIntPoint TargetCell);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerFireAtMothership(AAShip* Ship, AAMotherShip* Mothership);
            bool ServerFireAtMothership_Validate(AAShip* Ship, AAMotherShip* Mothership);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerPlayCard(UUCardData* Card, FIntPoint TargetCell);
            bool ServerPlayCard_Validate(UUCardData* Card, FIntPoint TargetCell);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerPlaceExpert(UUCardData* Card, AAMotherShip* Mothership);
            bool ServerPlaceExpert_Validate(UUCardData* Card, AAMotherShip* Mothership);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerActivateEffect(AAShip* Ship, int32 EffectIndex);
            bool ServerActivateEffect_Validate(AAShip* Ship, int32 EffectIndex);

        UFUNCTION(BlueprintCallable, Server, Reliable)
            void Server_RevealShip(AAShip* Ship);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void Server_GetAllDiscardsCards(bool bIsPlayerDiscardDeck);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerUpgrade(AAShip* Ship, UUCardData* Card);
            bool ServerUpgrade_Validate(AAShip* Ship, UUCardData* Card);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerPlaySabotage(UUCardData* Card, const FEffectContext& context);
            bool ServerPlaySabotage_Validate(UUCardData* Card, const FEffectContext& context);

        UFUNCTION(Server, Reliable)
            void ServerRequestReachableCells(AAShip* Ship);

        UFUNCTION(Server, Reliable)
            void ServerEndTurn();

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerSpawnShip(TSubclassOf<AAShip> Ship, FIntPoint TargetCell, UUCardData* cardData);
            bool ServerSpawnShip_Validate(TSubclassOf<AAShip> Ship, FIntPoint TargetCell, UUCardData* cardData);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerConfirmSabotageTarget(int32 ChosenIndex);
            bool ServerConfirmSabotageTarget_Validate(int32 ChosenIndex);

        UFUNCTION(Server, Reliable)
            void ServerRequestActivatableEffects(AAShip* Ship);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerConfirmEffectTarget(AAShip* TargetShip);
            bool ServerConfirmEffectTarget_Validate(AAShip* TargetShip);

        UFUNCTION(Server, Reliable, WithValidation)
            void ServerConfirmEffectTargetCell(FIntPoint Cell);
            bool ServerConfirmEffectTargetCell_Validate(FIntPoint Cell);

    UFUNCTION(Server, Reliable, WithValidation)
            void ServerResolveSabotageTarget(int32 CardIndex);
            bool ServerResolveSabotageTarget_Validate(int32 CardIndex);

    UFUNCTION(BlueprintCallable,Server, Reliable, WithValidation)
    void ServerActivateActiveEffect(AAShip* ShipSelected);
    bool ServerActivateActiveEffect_Validate(AAShip* ShipSelected);

    UFUNCTION(BlueprintCallable,Server, Reliable, WithValidation)
    void ServerReviveCard(int32 cardIndexInDump);
    bool ServerReviveCard_Validate(int32 cardIndexInDump);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerAskForEnemyEssence();

#pragma endregion

#pragma region ClientMethodes

    UFUNCTION(Client, Reliable)
        void ClientOnReachableCells(const TArray<FIntPoint>& Cells);

    UFUNCTION(Client, Reliable)
        void ClientReceiveActivatableEffects(const TArray<FActivatableEffectInfo>& Infos, AAShip* Ship, bool bHaveActiveEffect);

    UFUNCTION(Client, Reliable)
        void ClientPromptEffectTarget(EEffectTargetKind Kind);

    UFUNCTION(Client, Reliable)
        void ClientPromptSabotageTarget(const TArray<UUCardData*>& Candidates);

    UFUNCTION(Client, Reliable)
        void ClientOnTurnStarted(int32 ActivePlayerID);

    UFUNCTION(Client, Reliable)
        void ClientOnTurnEnded();

    UFUNCTION(Client, Reliable)
        void ClientOnPlayCard();

    UFUNCTION(Client, Reliable)
        void ClientUpdateHand(const TArray<UUCardData*>& NewHand);

    UFUNCTION(Client, Reliable)
        void ClientOnActionRejected(const FString& Reason);

    UFUNCTION(Client, Reliable)
        void ClientOnShipMoved(AAShip* Ship, FIntPoint NewCell);

    UFUNCTION(Client, Reliable)
        void ClientOnShipDestroyed(AAShip* Ship);

    UFUNCTION(Client, Reliable)
        void ClientOnEssenceChanged(int32 NewEssence, int32 MaxEssence);

    UFUNCTION(Client, Reliable)
        void ClientOnVictory(int32 WinnerID);

    UFUNCTION(Client, Reliable)
        void ClientInitializeInput();

    UFUNCTION(Client, Reliable)
        void ClientOnCardDrawn(UUCardData* Card);

    UFUNCTION(Client, Reliable)
        void ClientOnCardDiscarded(UUCardData* Card);

    UFUNCTION(Client, Reliable)
    void ClientGetAllDiscardCards(const TArray<UUCardData*>& CardDiscards);

    UFUNCTION(BlueprintCallable, Client, Reliable)
        void ClientClearSelection();

    void ClearPendingActivation();

    UFUNCTION(Client, Reliable)
        void ClientMothershipHealthChanged(int32 ownMothership, int32 enemyMothership);

    UFUNCTION(Client, Reliable)
        void ClientOnEnemyEssenceReceived(int32 essence);

#pragma endregion

#pragma region BlueprintImplementableEvents

    UFUNCTION(BlueprintImplementableEvent)
        void OnActivatableEffectsReceivedBP(const TArray<FActivatableEffectInfo>& Infos);

    UFUNCTION(BlueprintImplementableEvent)
        void OnCardDrawnBP(UUCardData* Card);

    UFUNCTION(BlueprintImplementableEvent)
        void InitializeInputBP();

    UFUNCTION(BlueprintImplementableEvent)
        void OnTurnStartedBP(int32 ActivePlayerID, bool bIsLocalPlayerTurn);

    UFUNCTION(BlueprintImplementableEvent)
        void OnTurnEndedBP();

    UFUNCTION(BlueprintImplementableEvent)
        void OnHandUpdatedBP(const TArray<UUCardData*>& NewHand);

    UFUNCTION(BlueprintImplementableEvent)
        void OnActionRejectedBP(const FString& Reason);

    UFUNCTION(BlueprintImplementableEvent)
        void OnEssenceChangedBP(int32 NewEssence, int32 NewMaxEssence);

    UFUNCTION(BlueprintImplementableEvent)
        void OnVictoryBP(int32 WinnerID);

    UFUNCTION(BlueprintImplementableEvent)
        void OnShipDestroyedBP(AAShip* Ship);

    UFUNCTION(BlueprintImplementableEvent)
        void OnCardDiscardedBP(UUCardData* cardData);

    UFUNCTION(BlueprintImplementableEvent)
        void OnCardPlayedBP();

    UFUNCTION(BlueprintImplementableEvent)
        void OnShipSelectedBP(AAShip* Ship, bool bHaveActiveEffect);

    UFUNCTION(BlueprintImplementableEvent)
    void OnShipUnselectedBP(AAShip* Ship);

    UFUNCTION(BlueprintImplementableEvent)
        void OnSabotageTargetPromptBP(const TArray<UUCardData*>& Candidates);

    UFUNCTION(BlueprintImplementableEvent)
        void OnEffectTargetPromptBP(EEffectTargetKind Kind);

    UFUNCTION(BlueprintImplementableEvent)
        void OnClearSelectionBP();

    UFUNCTION(BlueprintImplementableEvent)
        void GetAllDiscardCardsBP(const TArray<UUCardData*>& CardDiscards);

    UFUNCTION(BlueprintImplementableEvent)
        void OnMothershipHealthChangedBP(int32 ownMothership, int32 enemyMothership);

    UFUNCTION(BlueprintImplementableEvent)
        void OnEnemyEssenceReceived(int32 enemyEssence);

#pragma endregion

#pragma region vars

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerID)
        int32 PlayerID = -1;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
        TObjectPtr<UInputMappingContext> BoardMappingContext;

    UPROPERTY(BlueprintReadOnly)
        TObjectPtr<AAShip> SelectedShip;

    UPROPERTY(BlueprintReadWrite)
        TSubclassOf<AAShip> PendingShipClass;

    UPROPERTY(BlueprintReadWrite)
        TObjectPtr<UUCardData> PendingCardData;

    UPROPERTY(BlueprintReadOnly)
        bool bIsMyTurn = false;

    UPROPERTY(ReplicatedUsing = OnRep_BoardVisualiser)
        TObjectPtr<AABoardVisualiser> BoardVisualiser;

    UPROPERTY(BlueprintReadWrite)
        EActionIntent PendingIntent = EActionIntent::NONE;

    UPROPERTY()
        EEffectTargetKind PendingActivationTargetKind = EEffectTargetKind::None;

    UPROPERTY()
        TObjectPtr<AAShip> PendingActivationShip;

        int32 PendingActivationEffectIndex = -1;

        bool bWaitingForCellTarget = false;

    UPROPERTY(BlueprintReadWrite, Replicated)
    bool bIsRevivingACard = false;

    UPROPERTY(BlueprintReadWrite, Replicated)
    TArray<ECardType> TypeOfCardsThatCanBeRevive;

#pragma endregion

};
