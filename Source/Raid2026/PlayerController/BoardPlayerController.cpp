#include "BoardPlayerController.h"

void ABoardPlayerController::OnRep_PlayerID()
{
	
}

void ABoardPlayerController::SetPlayerID(int32 ID)
{
	PlayerID = ID;
}

void ABoardPlayerController::ClickOnShip(AAShip* selectedShip)
{
	// if (UUTurnManager->GetCurrentPlayer() != selectedShip->GetOwnerID())
	// {
	// 	return;
	// }
	// if (selectedShip == SelectedShip)
	// {
	// 	SelectedShip = nullptr;
	// 	return;
	// }
	//
	// SelectedShip = selectedShip;
}

void ABoardPlayerController::ClickOnCell(AABoardCell* selectCell)
{

	// if (!IsValid(SelectedShip) && !BoardManager->IsCellOccupied(selectCell->cellData.Pos))
	// {
	// 	BoardVisualiser->SpawnShip(BoardVisualiser->ShipClass, selectCell->cellData.Pos, CardData, UUTurnManager->GetCurrentPlayer());
	// 	return;
	// }
	// BoardManager->MoveShipTo(SelectedShip, selectCell->cellData.Pos, UUTurnManager->GetCurrentPlayer());
}

void ABoardPlayerController::RequestEndTurn()
{
}

void ABoardPlayerController::RequestPlayCard(UUCardData* Card, FIntPoint TargetCell)
{
}

void ABoardPlayerController::ServerMoveShip_Implementation(AAShip* Ship, FIntPoint TargetCell)
{
}

bool ABoardPlayerController::ServerMoveShip_Validate(AAShip* Ship, FIntPoint TargetCell)
{
	return true;
}

void ABoardPlayerController::ServerFireAt_Implementation(AAShip* Shooter, FIntPoint TargetCell)
{
}

bool ABoardPlayerController::ServerFireAt_Validate(AAShip* Shooter, FIntPoint TargetCell)
{
	return true;
}

void ABoardPlayerController::ServerPlayCard_Implementation(UUCardData* Card, FIntPoint TargetCell)
{
}

bool ABoardPlayerController::ServerPlayCard_Validate(UUCardData* Card, FIntPoint TargetCell)
{
	return true;
}

void ABoardPlayerController::ServerSpawnShip_Implementation(AAShip* Ship, FIntPoint TargetCell)
{
}

bool ABoardPlayerController::ServerSpawnShip_Validate(AAShip* Ship, FIntPoint TargetCell)
{
	return true;
}

void ABoardPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABoardPlayerController::HandleShipSelected(AAShip* Ship)
{
}

void ABoardPlayerController::HandleCellTargeted(AABoardCell* Cell)
{
}

void ABoardPlayerController::ClearSelection()
{
	SelectedShip = nullptr;
}

void ABoardPlayerController::ClientOnVictory_Implementation(int32 WinnerID)
{
}

void ABoardPlayerController::ClientOnEssenceChanged_Implementation(int32 NewEssence, int32 MaxEssence)
{
}

void ABoardPlayerController::ClientOnShipDestroyed_Implementation(AAShip* Ship)
{
}

void ABoardPlayerController::ClientOnShipMoved_Implementation(AAShip* Ship, FIntPoint NewCell)
{
}

void ABoardPlayerController::ClientOnActionRejected_Implementation(const FString& Reason)
{
}

void ABoardPlayerController::ClientUpdateHand_Implementation(const TArray<UUCardData*>& NewHand)
{
}

void ABoardPlayerController::ClientOnTurnEnded_Implementation()
{
}

void ABoardPlayerController::ClientOnTurnStarted_Implementation(int32 ActivePlayerID)
{
	SetPlayerID(ActivePlayerID);
}

void ABoardPlayerController::ServerEndTurn_Implementation()
{
}
