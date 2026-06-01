#include "UBoardManager.h"
#include "Raid2026/CoreLayer/FCell.h"

bool UUBoardManager::IsValidCell(FIntPoint Pos) const
{
	return Pos.X >= 0 && Pos.X < GridWidth && Pos.Y >= 0 && Pos.Y < GridHeight;
}

bool UUBoardManager::IsCellOccupied(FIntPoint Pos) const
{
	return false;
	//return IsValidCell(Pos) && Grid[Pos.X][Pos.Y].Occupant.IsValid();
}

FCell UUBoardManager::GetCell(FIntPoint Pos) const
{
	check(IsValidCell(Pos));
	return Grid[Pos.X][Pos.Y];
}

FCell UUBoardManager::GetCell(int32 X, int32 Y) const
{
	check(IsValidCell(FIntPoint(X, Y)));
	return Grid[X][Y];
}

// AShipActor* UUBoardManager::GetShipAt(FIntPoint Pos) const
// {
// 	return Grid[Pos.X][Pos.Y].Occupant;
// }

// AMothershipActor* UUBoardManager::GetMothershipAt(FIntPoint Pos) const
// {
// 	return Grid[Pos.X][Pos.Y].Occupant;
// }

bool UUBoardManager::IsMothershipCell(FIntPoint Pos, int32 ShooterPlayerID) const
{
	// if (Pos = GetMothershipAt(Pos).pos && GetMothershipAt(Pos).ID != ShooterPlayerID)
	// {
	// 	return true;
	// }
	return false;
}

TArray<FIntPoint> UUBoardManager::GetFreeSpawnCells(int32 PlayerID) const
{
	return TArray<FIntPoint>();
}

TArray<FReachableCell> UUBoardManager::GetReachableCells(AShipActor* Ship, int32 AvailableEssence) const
{
	TArray<FReachableCell> ReachableCells;
	for (int32 X = 0; X < GridWidth; X++)
	{
		for (int32 Y = 0; Y < GridHeight; Y++)
		{
			// if (Grid[X][Y]
			// }
		}
	}
	return ReachableCells;
}

bool UUBoardManager::IsLineOfSight(FIntPoint From, FIntPoint To) const
{
	return false;
}

int32 UUBoardManager::CheckRefineries()
{
	return 0;
}

// bool UUBoardManager::MoveShipTo(AShipActor* Ship, FIntPoint TargetCell)
// {
// 	if (Grid[TargetCell.X][TargetCell.Y].IsEmpty())
// 	{
// 		SetOccupant(TargetCell, Ship);
// 		return true;
// 	}
// 	return false;
// }

// void UUBoardManager::RemoveShipFromGrid(AShipActor* Ship)
// {
// 	ClearOccupant(Ship.pos);
// }

void UUBoardManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUBoardManager::Deinitialize()
{
	Super::Deinitialize();
}

FCell& UUBoardManager::GetCellRef(FIntPoint Pos)
{
	check(IsValidCell(Pos));
	return Grid[Pos.X][Pos.Y];
}

// void UUBoardManager::SetOccupant(FIntPoint Pos, AHordeActor* Actor)
// {
// 	Grid[Pos.X][Pos.Y].Occupant = Actor;
// }

void UUBoardManager::ClearOccupant(FIntPoint Pos)
{
	check(IsValidCell(Pos));
	//Grid[Pos.X][Pos.Y].Occupant.Reset();
}
