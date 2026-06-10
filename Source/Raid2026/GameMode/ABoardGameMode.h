#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../SubSystem/CombatResolver.h"
#include "../SubSystem/DeckManager.h"
#include "../SubSystem/UTurnManager.h"
#include "../SubSystem/UBoardManager.h"
#include "../PlayerController/BoardPlayerController.h"
#include "../Actor/ABoardCell.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TObjectPtr<UUCardData>> testDeck;

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void HandleMoveShip(ABoardPlayerController* playerInstigator, AAShip* ship, FIntPoint targetCell);

	void HandleFireAt(ABoardPlayerController* playerInstigator, AAShip* shooter, FIntPoint targetCell);

	void HandlePlayCard(ABoardPlayerController* playerInstigator, UUCardData* Card, FIntPoint TargetCell);

	void HandleEndTurn(ABoardPlayerController* playerInstigator);

	void HandleSpawnShip(ABoardPlayerController* playerInstigator, TSubclassOf<AAShip> ShipClass, FIntPoint TargetCell, UUCardData* CardData);

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUBoardManager> boardManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUTurnManager> turnManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UDeckManager> deckManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCombatResolver> combatResolverClass;

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

	void OnVictoryConditionMet(int32 WinnerPlayerID);

	void OnRefineryControlChanged(AARefinery* Refinery, int32 NewOwnerID);

	TArray<FIntPoint> GetReachableCellsForShip(ABoardPlayerController* Instigator, AAShip* Ship);
};
