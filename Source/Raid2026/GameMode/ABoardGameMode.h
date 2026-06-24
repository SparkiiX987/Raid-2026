#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../SubSystem/CombatResolver.h"
#include "../SubSystem/DeckManager.h"
#include "../SubSystem/UTurnManager.h"
#include "../SubSystem/UBoardManager.h"
#include "../SubSystem/UpgradesManager.h"
#include "../PlayerController/BoardPlayerController.h"
#include "../Actor/ABoardCell.h"
#include "Raid2026/SubSystem/UPathFinder.h"
#include <Raid2026/SubSystem/EffectManager.h>
#include "ABoardGameMode.generated.h"

UCLASS()
class RAID2026_API AABoardGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 playerCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<int32, TObjectPtr<ABoardPlayerController>> connectedPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextPlayerID;

	TMap<int32, TArray<TObjectPtr<UUCardData>>> testDeck;

	virtual void BeginPlay() override;
	virtual void HandleSeamlessTravelPlayer(AController*& Controller) override;
	virtual void PostSeamlessTravel() override;
	virtual void Logout(AController* Exiting) override;

	void HandleMoveShip(ABoardPlayerController* playerInstigator, AAShip* ship, FIntPoint targetCell);

	void HandleFireAtMothership(ABoardPlayerController* playerInstigator, AAShip* ship, AAMotherShip* TargetMothership);

	void HandleFireAt(ABoardPlayerController* playerInstigator, AAShip* shooter, FIntPoint targetCell);

	void HandlePlayCard(ABoardPlayerController* playerInstigator, UUCardData* Card, FIntPoint TargetCell);

	void HandleEndTurn(ABoardPlayerController* playerInstigator);

	void HandleSpawnShip(ABoardPlayerController* playerInstigator, TSubclassOf<AAShip> ShipClass, FIntPoint TargetCell, UUCardData* CardData);

	void HandlePlaceExpert(ABoardPlayerController* playerInstigator, UUCardData* CardData);

	void HandlePlaceUpgrade(ABoardPlayerController* playerInstigator, UUCardData* CardData, AAShip* ship);

	void HandleRemoveUpgrade(UUpgrade* upgrade, AAShip* ship);

	UFUNCTION()
		void HandleTurnStarted(int32 PlayerID, int32 TurnNumber);

	UFUNCTION()
		void HandleEssenceSpent(int32 PlayerId, int32 Amount, bool bWasBonus);

	UFUNCTION()
		void HandleBonusEssenceGained(int32 PlayerId, int32 Amount);

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUBoardManager> boardManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUTurnManager> turnManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UDeckManager> deckManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCombatResolver> combatResolverClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UEffectManager> effectManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUpgradesManager> upgradeManagerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPathFinder> pathFinderClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AABoardVisualiser> boardVisualiserClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UUBoardManager> boardManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UUTurnManager> turnManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UDeckManager> deckManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UCombatResolver> combatResolver;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UEffectManager> effectManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UUpgradesManager> upgradesManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UPathFinder> PathFinder;

	UPROPERTY()
		TObjectPtr<AABoardVisualiser> boardVisualiser;

	void PlayerSetupFinished(ABoardPlayerController* PC, int32 playerId);

	void SpawnManagers();

	void SpawnBoardActors();

	void SpawnCellActors();

	void SpawnVisualiser();

	UFUNCTION(BlueprintCallable)
		void StartGameWhenReady();

	void StartGame();

	bool ValidateIsPlayerTurn(ABoardPlayerController* playerInstigator) const;

	bool ValidateShipOwnership(ABoardPlayerController* playerInstigator, AAShip* Ship) const;

	void RejectAction(ABoardPlayerController* playerInstigator, const FString& Reason) const;

	UFUNCTION(BlueprintCallable)
		void BroadcastTurnStarted(int32 PlayerId);

	void BroadcastEssenceChanged(int32 playerId);
	void SyncHandToPlayer(int32 PlayerId);

	void UpdateGridState();

	void BroadcastFireResult(const FFireResult& Result);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AARefinery> refineryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AAMotherShip> mothershipClass;

	void CheckVictoryConditions();

	UFUNCTION(BlueprintImplementableEvent)
		void OnInitialisationFinishedBP();

	UFUNCTION(BlueprintImplementableEvent)
		void OnSpawnFinishedBP();

	void OnVictoryConditionMet(int32 WinnerPlayerID);

	void OnRefineryControlChanged(AARefinery* Refinery, int32 NewOwnerID);

	TArray<FIntPoint> GetReachableCellsForShip(ABoardPlayerController* Instigator, AAShip* Ship);

	UFUNCTION()
		void HandleCardDrawn(int32 PlayerId, UUCardData* Card);
	
	AABoardGameMode();
	virtual void Tick(float DeltaTime) override;
};
