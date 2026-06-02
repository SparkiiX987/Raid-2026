#include "UBoardManager.h"
#include "Raid2026/CoreLayer/FCell.h"
#include "Raid2026/CoreLayer/FReachableCell.h"
//#include "Raid2026/CoreLayer/TurnManager.h"

void UUBoardManager::InitializeBoard(const TArray<AActor*>& Refineries, AActor* MothershipP0,
                                     AActor* MothershipP1)
{
	for (int32 X = 0; X < GridWidth; X++)
	{
		for (int32 Y = 0; Y < GridHeight; Y++)
		{
			Grid[X][Y] = FCell();
			if (GEngine)
			{
				FString text = FString::Printf(TEXT("message"));

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
			}
		}
	}
	
	GetCellRef(FIntPoint(3,3)).Type = ECellType::Refinery;
	GetCellRef(FIntPoint(3,5)).Type = ECellType::Refinery;
	
	for (int i = 0; i < GridWidth; i++)
	{
		GetCellRef(FIntPoint(i,1)).Type = ECellType::SpawnP0;
		GetCellRef(FIntPoint(i,GridHeight-2)).Type = ECellType::SpawnP1;
		// SetOccupant(FIntPoint(i, 0), MothershipP0);
		// SetOccupant(FIntPoint(i, GridHeight - 1), MothershipP1);
	}
	//
	// RefineryActors = Refineries;
	// Motherships[0] = MothershipP0;
	// Motherships[1] = MothershipP1;


}

bool UUBoardManager::IsValidCell(FIntPoint Pos) const
{
	return Pos.X >= 0 && Pos.X < GridWidth && Pos.Y >= 0 && Pos.Y < GridHeight;
}

// bool UUBoardManager::IsCellOccupied(FIntPoint Pos) const
// {
// 	return IsValidCell(Pos) && GetCell(Pos).Occupant.IsValid();
// }

FCell UUBoardManager::GetCell(FIntPoint Pos) const
{
	check(IsValidCell(Pos));
	return Grid[Pos.X][Pos.Y];
}

FCell UUBoardManager::GetCells(int32 X, int32 Y) const
{
	check(IsValidCell(FIntPoint(X, Y)));
	return Grid[X][Y];
}

// AShipActor* UUBoardManager::GetShipAt(FIntPoint Pos) const
// {
// 	if (AShipActor* Ship = Cast<AShipActor>(GetCell(Pos).Occupant.Get()))
// 	{
// 		return Ship;
// 	}
// 	return nullptr;
// }

// AMothershipActor* UUBoardManager::GetMothershipAt(FIntPoint Pos) const
// {
// 	if (GetCell(Pos).Occupant == Motherships[0])
// 		{
// 		return Motherships[0];
// 	}
//
// 	if (GetCell(Pos).Occupant == Motherships[1])
// 	{
// 		return Motherships[1];
// 	}
// 	return nullptr;
// }

// bool UUBoardManager::IsMothershipCell(FIntPoint Pos, int32 ShooterPlayerID) const
// {
// 	if (GetMothershipAt(Pos) != nullptr && GetMothershipAt(Pos)->ownerPlayer != ShooterPlayerID)
// 	{
// 		return true;
// 	}
// 	return false;
// }

// TArray<FIntPoint> UUBoardManager::GetFreeSpawnCells(int32 PlayerID) const
// {
// 	TArray<FIntPoint> FreeSpawnCells;
// 	ECellType CellTypeWhereSpawn;
// 	
// 	if (PlayerID == 0) CellTypeWhereSpawn = ECellType::SpawnP0;
// 	else CellTypeWhereSpawn = ECellType::SpawnP1;
// 	
// 	for (int32 X = 0; X < GridWidth; X++)
// 	{
// 		for (int32 Y = 0; Y < GridHeight; Y++)
// 		{
// 			if (Grid[X][Y].Type == CellTypeWhereSpawn && !IsCellOccupied(FIntPoint(X, Y)))
// 			{
// 				FreeSpawnCells.Add(FIntPoint(X, Y));
// 			}
// 		}
// 	}
// 	return FreeSpawnCells;
// }

// TArray<FReachableCell> UUBoardManager::GetReachableCells(AShipActor* Ship, int32 AvailableEssence) const
// {
// 	TArray<FReachableCell> ReachableCells;
// 	for (int32 X = -AvailableEssence; X <= AvailableEssence; X++)
// 	{
// 		for (int32 Y = -AvailableEssence; Y <= AvailableEssence; Y++)
// 		{
// 			FIntPoint Target = Ship->gridPosition + FIntPoint(X,Y);
// 			int32 Cost = FMath::Abs(X) + FMath::Abs(Y);
// 			
// 			if (!IsValidCell(Target) || X == 0 && Y == 0 || Cost > AvailableEssence)
// 			{
// 				continue;
// 			}
// 			
// 			if (!IsCellOccupied(FIntPoint(Target)))
// 			{
// 				FReachableCell ReachableCell;
// 				ReachableCell.Cell = Ship->gridPosition + FIntPoint(X, Y);
// 				ReachableCell.EssenceCost = FMath::Abs(X) + FMath::Abs(Y);	
// 				ReachableCells.Add(ReachableCell);
// 			}
// 			
// 		}
// 	}
// 	return ReachableCells;
// }

// bool UUBoardManager::IsLineOfSight(FIntPoint From, FIntPoint To) const
// {
// 	if (From == To || From.X != To.X && From.Y != To.Y)
// 	{
// 		return false;
// 	}
//
// 	const FIntPoint Direction(FMath::Sign(To.X - From.X),FMath::Sign(To.Y - From.Y));
//
// 	for (FIntPoint Pos = From + Direction; Pos != To; Pos += Direction)
// 	{
// 		if (IsCellOccupied(Pos))
// 		{
// 			return false;
// 		}
// 	}
// 	return true;
// }

// int32 UUBoardManager::CheckRefineries(int32 playerID)
// {
// 	int32 RefineriesPossessed = 0;
// 	for (int32 X = 0; X < RefineryActors.Num(); X++)
// 	{
// 		if (RefineryActors[X]->ControllerPlayerID == playerID)
// 		{
// 			RefineriesPossessed++;
// 		}
// 	}
// 	return RefineriesPossessed;
// }

// bool UUBoardManager::MoveShipTo(AShipActor* Ship, FIntPoint TargetCell, int32 playerID)
// {
// 	TArray<FReachableCell> ReachableCells = GetReachableCells(Ship, TurnManager->GetAvailable(playerID));
//
// 	bool bReachable = ReachableCells.ContainsByPredicate(
// 		[&](const FReachableCell& Cell)
// 		{
// 			return Cell.Cell == TargetCell;
// 		});
// 	
// 	if (bReachable && !IsCellOccupied(TargetCell))
// 	{
// 		ClearOccupant(Ship->gridPosition);
// 		SetOccupant(TargetCell, Ship);
// 		return true;
// 	}
// 	return false;
// }

// bool UUBoardManager::PlaceShip(AShipActor* Ship, FIntPoint target, int32 PlayerID)
// {
// 	if (!GetFreeSpawnCells(PlayerID).Contains(target))
// 	{
// 		return false;
// 	}
// 	SetOccupant(target, Ship);
// 	return true;
// }
//
// void UUBoardManager::RemoveShipFromGrid(AShipActor* Ship)
// {
// 	ClearOccupant(Ship->gridPosition);
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

// void UUBoardManager::SetOccupant(FIntPoint Pos, ABoardActor* Actor)
// {
// 	check(IsValidCell(Pos));
// 	Grid[Pos.X][Pos.Y].Occupant = Actor;
// 	Actor->gridPosition = Pos;
// }

// void UUBoardManager::ClearOccupant(FIntPoint Pos)
// {
// 	check(IsValidCell(Pos));
// 	Grid[Pos.X][Pos.Y].Occupant.Reset();
// }
