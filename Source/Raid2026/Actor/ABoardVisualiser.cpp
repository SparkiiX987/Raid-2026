#include "ABoardVisualiser.h"
#include "../GameState/BoardGameState.h"

AABoardVisualiser::AABoardVisualiser()
{
	bReplicates = true;
}

void AABoardVisualiser::BeginPlay()
{
	Super::BeginPlay();
	TryBindToGameState();
}

void AABoardVisualiser::TryBindToGameState()
{
	ABoardGameState* GS = GetWorld()->GetGameState<ABoardGameState>();
	if (GS)
	{
		GS->OnGridStateChanged.AddDynamic(
			this, &AABoardVisualiser::OnGridStateChanged);
		GS->OnActivePlayerChanged.AddDynamic(
			this, &AABoardVisualiser::OnActivePlayerChanged);
		GS->OnGameOver.AddDynamic(
			this, &AABoardVisualiser::OnGameOver);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AABoardVisualiser::TryBindToGameState);
	}
}

void AABoardVisualiser::SpawnCellActors()
{
	UWorld* world = GetWorld();

	if (!world || !CellActorClass)
	{
		return;
	}

	FVector Location(0.f, 0.f, -100.f);
	const FRotator Rotation = FRotator::ZeroRotator;

	for (int32 X = 0; X < UUBoardManager::GridWidth; X++)
	{
		for (int32 Y = 1; Y < UUBoardManager::GridHeight + 1; Y++)
		{
			Location.X = X * BoardManager->CellGap;
			Location.Y = Y * BoardManager->CellGap;

			AABoardCell* Cell = world->SpawnActor<AABoardCell>(
				CellActorClass,
				Location,
				Rotation);

			if (!Cell)
			{
				continue;
			}
			Cell->SetActorScale3D(FVector(CellSize, CellSize, CellSize));

			FIntPoint cellGridPos = WorldToGrid(Location);

			Cell->cellData.Pos = cellGridPos;
			Cell->cellData.Type = BoardManager->GetCell(cellGridPos).Type;
			if (BoardManager->GetCell(cellGridPos).Occupant)
			{
				Cell->cellData.Occupant = BoardManager->GetCell(cellGridPos).Occupant;
			}

			CellActors.Add(Cell);
		}
	}
}

void AABoardVisualiser::OnGridStateChanged(const TArray<FReplicatedCellState>& NewGrid)
{
	for (const FReplicatedCellState& Cell : NewGrid)
	{
		AABoardCell* CellActor = GetCellActor(Cell.pos);
		if (CellActor)
			CellActor->SetRefineryOwner(Cell.RefineryOwnerId);

		if (Cell.bHasShip && BoardManager)
		{
			AAShip* Ship = BoardManager->GetShipAt(Cell.pos);
			if (Ship)
			{
				FVector WorldPos = GridToWorld(Cell.pos);
				Ship->SetActorLocation(WorldPos);
			}
		}
	}
}

void AABoardVisualiser::OnActivePlayerChanged(int32 newActivePlayer)
{

}

void AABoardVisualiser::OnGameOver(int32 winner)
{

}

AABoardCell* AABoardVisualiser::GetCellActor(FIntPoint GridPos) const
{
	return CellActors[CellIndex(GridPos.X, GridPos.Y)];
}

FVector AABoardVisualiser::GridToWorld(FIntPoint GridPos) const
{
	return FVector(GridPos.X*BoardManager->CellGap, GridPos.Y*BoardManager->CellGap, 0.0f);
}

FIntPoint AABoardVisualiser::WorldToGrid(FVector WorldPos) const
{
	return FIntPoint(FMath::RoundToInt(WorldPos.X / BoardManager->CellGap), FMath::RoundToInt(WorldPos.Y / BoardManager->CellGap));
}

void AABoardVisualiser::HighlightCells(TArray<FIntPoint> Cells)
{
	for (int i = 0; i < Cells.Num(); i++)
	{
		AABoardCell* cell = GetCellActor(Cells[i]);

		if (!IsValid(cell)) continue;

		cell->HighlightCell();
		highlitedCells.Add(cell);
	}
}

void AABoardVisualiser::ClearHighlights()
{
	for (AABoardCell* cell : highlitedCells)
	{
		cell->ClearCellHighlight();
	}

	highlitedCells.Empty();
}

int32 AABoardVisualiser::CellIndex(int32 X, int32 Y) const
{
	for (int32 x = 0; x < CellActors.Num(); x++)
	{
		if (CellActors[x]->cellData.Pos == FIntPoint(X, Y))
		{
			return x;
		}
	}
	return -1;
}
