#include "UBoardManager.h"

#include "UTurnManager.h"
#include "../CoreLayer/Cells/FReachableCell.h"
#include "Raid2026/Actor/ARefinery.h"
#include "Raid2026/Actor/AShip.h"


void UUBoardManager::InitializeBoard(const TArray<AARefinery*>& Refineries,
                                     AAMotherShip* MothershipP0,
                                     AAMotherShip* MothershipP1)
{
	for (int32 X = 0; X < GridWidth; X++)
	{
		for (int32 Y = 0; Y <= GridHeight + 1; Y++)
		{
			FCell& Cell = Grid[X][Y];

			Cell = FCell();
			Cell.Pos = FIntPoint(X, Y);
		}
	}

	GetCellRef(FIntPoint(2, 3)).Type = ECellType::Refinery;
	SetOccupant(FIntPoint(2, 3), Refineries[0]);
	RefineryActors.Add(Refineries[0]);

	GetCellRef(FIntPoint(2, 5)).Type = ECellType::Refinery;
	SetOccupant(FIntPoint(2, 5), Refineries[1]);
	RefineryActors.Add(Refineries[1]);

	for (int32 X = 0; X < GridWidth; X++)
	{
		GetCellRef(FIntPoint(X, 1)).Type = ECellType::SpawnP0;
		GetCellRef(FIntPoint(X, GridHeight)).Type = ECellType::SpawnP1;
		SetOccupant(FIntPoint(X, 0), MothershipP0);
		SetOccupant(FIntPoint(X, GridHeight+1), MothershipP1);
	}
	SetOccupant(FIntPoint(GridWidth/2, 0), MothershipP0);
	SetOccupant(FIntPoint(GridWidth/2, GridHeight+1), MothershipP1);
	Motherships.Add(0,MothershipP0);
	Motherships.Add(1,MothershipP1);
}

bool UUBoardManager::IsValidCell(FIntPoint Pos) const
{
	return Pos.X >= 0 && Pos.X < GridWidth && Pos.Y >= 0 && Pos.Y < GridHeight+2;
}

bool UUBoardManager::IsCellOccupied(FIntPoint Pos) const
{
	return IsValidCell(Pos) && IsValid(GetCell(Pos).Occupant);
}

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

AAShip* UUBoardManager::GetShipAt(FIntPoint Pos) const
{
	if (AAShip* Ship = Cast<AAShip>(GetCell(Pos).Occupant.Get()))
	{
		return Ship;
	}
	return nullptr;
}

AAMotherShip* UUBoardManager::GetMothershipAt(FIntPoint Pos) const
{
	AAMotherShip* mothership = Cast<AAMotherShip>(GetCell(Pos).Occupant);

	if (!IsValid(mothership)) return nullptr;

	if (mothership == Motherships[0])
	{
		return Motherships[0];
	}

	if (mothership == Motherships[1])
	{
		return Motherships[1];
	}

	return nullptr;
}

bool UUBoardManager::IsMothershipCell(FIntPoint Pos, int32 ShooterPlayerID) const
{
	if (GetMothershipAt(Pos) != nullptr && GetMothershipAt(Pos)->ownerPlayer != ShooterPlayerID)
	{
		return true;
	}
	return false;
}

TArray<FIntPoint> UUBoardManager::GetFreeSpawnCells(int32 PlayerID) const
{
	TArray<FIntPoint> FreeSpawnCells;
	ECellType CellTypeWhereSpawn;
	
	if (PlayerID == 0) CellTypeWhereSpawn = ECellType::SpawnP0;
	else CellTypeWhereSpawn = ECellType::SpawnP1;
	
	for (int32 X = 0; X < GridWidth; X++)
	{
		for (int32 Y = 1; Y <= GridHeight; Y++)
		{
			if (Grid[X][Y].Type == CellTypeWhereSpawn && !IsCellOccupied(FIntPoint(X, Y)))
			{
				FreeSpawnCells.Add(FIntPoint(X, Y));
			}
		}
	}
	return FreeSpawnCells;
}

TArray<FReachableCell> UUBoardManager::GetReachableCells(AAShip* Ship, int32 AvailableEssence) const
{
	if (Ship == nullptr) return TArray<FReachableCell>();
	TArray<FReachableCell> ReachableCells;
	for (int32 X = -AvailableEssence; X <= AvailableEssence; X++)
	{
		for (int32 Y = -AvailableEssence; Y <= AvailableEssence; Y++)
		{
			FIntPoint Target = Ship->gridPosition + FIntPoint(X,Y);
			int32 TotalMove = FMath::Abs(X) + FMath::Abs(Y);
			
			if (!IsValidCell(Target) || X == 0 && Y == 0 || TotalMove > AvailableEssence || TotalMove > Ship->GetCurrentSpeed())
			{
				continue;
			}
			
			if (!IsCellOccupied(FIntPoint(Target)))
			{
				FReachableCell ReachableCell;
				ReachableCell.Cell = Ship->gridPosition + FIntPoint(X, Y);
				ReachableCell.EssenceCost = FMath::Abs(X) + FMath::Abs(Y);	
				ReachableCells.Add(ReachableCell);
			}
			
		}
	}
	return ReachableCells;
}

bool UUBoardManager::IsLineOfSight(FIntPoint From, FIntPoint To) const
{
	if (From == To || From.X != To.X && From.Y != To.Y)
	{
		return false;
	}

	const FIntPoint Direction(FMath::Sign(To.X - From.X),FMath::Sign(To.Y - From.Y));

	for (FIntPoint Pos = From + Direction; Pos != To; Pos += Direction)
	{
		if (IsCellOccupied(Pos))
		{
			return false;
		}
	}
	return true;
}

int32 UUBoardManager::CheckRefineries(int32 playerID)
{
	int32 RefineriesPossessed = 0;
	for (int32 X = 0; X < RefineryActors.Num(); X++)
	{
		if (RefineryActors[X]->ControllerPlayerID == playerID)
		{
			RefineriesPossessed++;
		}
	}
	return RefineriesPossessed;
}

bool UUBoardManager::MoveShipTo(AAShip* Ship, FIntPoint TargetCell, int32 playerID)
{
	if (IsCellOccupied(TargetCell) || Ship->ownerPlayer != playerID)
		return false;

	ClearOccupant(Ship->gridPosition);
	Ship->bHasMoved = true;
	SetOccupant(TargetCell, Ship);
	return true;
}

void UUBoardManager::PlaceShip(AAShip* Ship, FIntPoint target)
{
	SetOccupant(target, Ship);
}

void UUBoardManager::RemoveShipFromGrid(AAShip* Ship)
{
	ClearOccupant(Ship->gridPosition);
}

FCell& UUBoardManager::GetCellRef(FIntPoint Pos)
{
	return Grid[Pos.X][Pos.Y];
}

void UUBoardManager::SetOccupant(FIntPoint Pos, AABoardActor* Actor)
{
	GetCellRef(Pos).Occupant = Actor;
	Actor->gridPosition = Pos;

	FVector WorldPos = FVector(Pos.X * CellGap, Pos.Y * CellGap, 0.f);
	Actor->SetActorLocation(WorldPos);
}

void UUBoardManager::ClearOccupant(FIntPoint Pos)
{
	Grid[Pos.X][Pos.Y].Occupant = nullptr;
}
