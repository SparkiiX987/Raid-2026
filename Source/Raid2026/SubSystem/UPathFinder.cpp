#include "UPathFinder.h"

#include "UBoardManager.h"
#include "Containers/Queue.h"
#include "Containers/Set.h"
#include "Containers/Map.h"
#include "Algo/Reverse.h"

const TArray<FIntPoint> UPathFinder::Directions =
{
	FIntPoint(1, 0),
	FIntPoint(-1, 0),
	FIntPoint(0, 1),
	FIntPoint(0, -1)
};

TArray<FIntPoint> UPathFinder::InitializeCheck(AAShip* Ship, FCell CellToCheck)
{
	PathTaken.Reset();

	shipSpeed = Ship->GetCurrentSpeed();

	if (CellToCheck.Occupant != nullptr)
	{
		return PathTaken;
	}

	cellSelect = CellToCheck.Pos;

	SearchPath(Ship->gridPosition, Ship);
	if (!PathTaken.IsEmpty())
	{
		PathCost = (PathTaken.Num() - 1) * Ship->CardData->stats.moveCost;

		if (PathCost > TurnManager->GetCurrentEssence(TurnManager->GetCurrentPlayer()))
		{
			PathTaken.Reset();
		}
	}
	return PathTaken;
}

void UPathFinder::SearchPath(FIntPoint StartCell, AAShip* Ship)
{
	TQueue<FIntPoint> OpenList;

	TSet<FIntPoint> Visited;

	TMap<FIntPoint, FIntPoint> Parent;

	TMap<FIntPoint, int32> Cost;

	OpenList.Enqueue(StartCell);
	Visited.Add(StartCell);

	Cost.Add(StartCell, 0);

	bool bFoundPath = false;

	while (!OpenList.IsEmpty())
	{
		FIntPoint CurrentCell;
		OpenList.Dequeue(CurrentCell);

		if (CurrentCell == cellSelect)
		{
			bFoundPath = true;
			break;
		}

		const int32 CurrentCost = Cost[CurrentCell];

		for (const FIntPoint& Direction : Directions)
		{
			const FIntPoint NewCell = CurrentCell + Direction;

			const int32 NewCost = CurrentCost + 1;

			if (NewCost > shipSpeed)
			{
				continue;
			}

			if (!BoardManager->IsValidCell(NewCell)
				|| Visited.Contains(NewCell)
				|| (BoardManager->IsCellOccupied(NewCell) && NewCell != cellSelect))
			{
				continue;
			}

			Visited.Add(NewCell);

			Parent.Add(NewCell, CurrentCell);

			Cost.Add(NewCell, NewCost);

			OpenList.Enqueue(NewCell);
		}
	}

	if (!bFoundPath)
	{
		return;
	}

	BuildPath(StartCell, Parent);
}

void UPathFinder::BuildPath(FIntPoint StartCell,const TMap<FIntPoint, FIntPoint>& Parent)
{
	PathTaken.Reset();

	FIntPoint Current = cellSelect;

	while (Current != StartCell)
	{
		PathTaken.Add(Current);

		const FIntPoint* ParentCell = Parent.Find(Current);

		if (!ParentCell)
		{
			PathTaken.Reset();
			return;
		}

		Current = *ParentCell;
	}

	PathTaken.Add(StartCell);

	Algo::Reverse(PathTaken);
}

TArray<FIntPoint> UPathFinder::GetAllCellAroundShip(AAShip* Ship)
{
	ReachableCell.Reset();

	const FIntPoint StartCell = Ship->gridPosition;

	TQueue<FIntPoint> OpenList;

	TSet<FIntPoint> Visited;

	TMap<FIntPoint, int32> Cost;

	OpenList.Enqueue(StartCell);

	Visited.Add(StartCell);

	Cost.Add(StartCell, 0);

	while (!OpenList.IsEmpty())
	{
		FIntPoint CurrentCell;
		OpenList.Dequeue(CurrentCell);

		const int32 CurrentCost = Cost[CurrentCell];

		for (const FIntPoint& Direction : Directions)
		{
			const FIntPoint NewCell = CurrentCell + Direction;

			const int32 NewCost = CurrentCost + 1;

			if (NewCost > Ship->GetCurrentSpeed()
				||!BoardManager->IsValidCell(NewCell)
				|| Visited.Contains(NewCell)
				|| BoardManager->IsCellOccupied(NewCell))
			{
				continue;
			}
			Visited.Add(NewCell);

			Cost.Add(NewCell, NewCost);

			ReachableCell.Add(NewCell);

			OpenList.Enqueue(NewCell);
		}
	}

	return ReachableCell;
}
