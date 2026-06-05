#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Raid2026/Actor/ABoardCell.h"
#include "Raid2026/Actor/ABoardVisualiser.h"
#include "Raid2026/Actor/AShip.h"
#include "BoardPlayerController.generated.h"

UCLASS()
class RAID2026_API ABoardPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PlayerID)
	int32 PlayerID = -1;

	UFUNCTION()
	void OnRep_PlayerID();

	void SetPlayerID(int32 ID);
	
	UPROPERTY(BlueprintReadOnly)
	AAShip* SelectedShip;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMyTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AABoardVisualiser> BoardVisualiser;

	UFUNCTION(BlueprintCallable)
	void ClickOnShip(AAShip* selectShip);
	
	UFUNCTION(BlueprintCallable)
	void ClickOnCell(AABoardCell* selectedCell);

	UFUNCTION(BlueprintCallable)
	void RequestEndTurn();

	UFUNCTION(BlueprintCallable)
	void RequestPlayCard(UUCardData* Card, FIntPoint TargetCell);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMoveShip(AAShip* Ship, FIntPoint TargetCell);
	bool ServerMoveShip_Validate(AAShip* Ship, FIntPoint TargetCell);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireAt(AAShip* Shooter, FIntPoint TargetCell);
	bool ServerFireAt_Validate(AAShip* Shooter, FIntPoint TargetCell);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayCard(UUCardData* Card, FIntPoint TargetCell);
	bool ServerPlayCard_Validate(UUCardData* Card, FIntPoint TargetCell);

	UFUNCTION(Server, Reliable)
	void ServerEndTurn();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnShip(AAShip* Ship, FIntPoint TargetCell);
	bool ServerSpawnShip_Validate(AAShip* Ship, FIntPoint TargetCell);

	UFUNCTION(Client, Reliable)
	void ClientOnTurnStarted(int32 ActivePlayerID);

	UFUNCTION(Client, Reliable)
	void ClientOnTurnEnded();

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool bWaitingForCellTarget = false;
	//EActionIntent PendingIntent = EActionIntent::None;

	void HandleShipSelected(AAShip* Ship);
	void HandleCellTargeted(AABoardCell* Cell);
	void ClearSelection();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTurnStartedBP(int32 ActivePlayerID, bool bIsLocalPlayerTurn);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHandUpdatedBP(const TArray<UUCardData*>& NewHand);

	UFUNCTION(BlueprintImplementableEvent)
	void OnActionRejectedBP(const FString& Reason);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEssenceChangedBP(int32 NewEssence, int32 MaxEssence);

	UFUNCTION(BlueprintImplementableEvent)
	void OnVictoryBP(int32 WinnerID);
};


