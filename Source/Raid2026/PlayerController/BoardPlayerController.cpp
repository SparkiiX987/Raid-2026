#include "BoardPlayerController.h"

void ABoardPlayerController::ClickOnShip(AAShip* selectedShip)
{
	if (UUTurnManager->GetCurrentPlayer() != selectedShip->GetOwnerID())
	{
		return;
	}
	if (selectedShip == SelectedShip)
	{
		SelectedShip = nullptr;
		return;
	}

	SelectedShip = selectedShip;
}

void ABoardPlayerController::ClickOnCell(AABoardCell* selectCell)
{

	if (!IsValid(SelectedShip) && !BoardManager->IsCellOccupied(selectCell->cellData.Pos))
	{
		BoardVisualiser->SpawnShip(BoardVisualiser->ShipClass, selectCell->cellData.Pos, CardData, UUTurnManager->GetCurrentPlayer());
		return;
	}
	BoardManager->MoveShipTo(SelectedShip, selectCell->cellData.Pos, UUTurnManager->GetCurrentPlayer());
}