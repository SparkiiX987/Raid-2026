#include "ABoardVisualiser.h"

void AABoardVisualiser::SpawnBoard(TArray<AActor*>& OutRefineries, AActor*& OutMothershipP0, AActor*& OutMothershipP1)
{
	SpawnCellActors();
	SpawnRefineries(OutRefineries);
	SpawnMotherships(OutMothershipP0, OutMothershipP1);
}

void AABoardVisualiser::DebugSpawnShipInAllCells()
{
	for (int32 X = 0; X < UUBoardManager::GridWidth; X++)
	{
		for (int32 Y = 0; Y < UUBoardManager::GridHeight+1; Y++)
		{
			if (BoardManager->IsCellOccupied(FIntPoint(X, Y)))
			{
				continue;
			}
			FVector Location(0.f, 0.f, 500.f);
			const FRotator Rotation = FRotator::ZeroRotator;

			AActor* ShipTest = GetWorld()->SpawnActor<AActor>(
				ShipClassTest,
				Location,
				Rotation);

			if (!ShipTest)
			{
				return;
			}
			BoardManager->SetOccupant(FIntPoint(X, Y), ShipTest);
		}
	}
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

void AABoardVisualiser::SpawnCellActors()
{
	if (!GetWorld() || !CellActorClass)
	{
		return;
	}

	FVector Location(0.f, 0.f, 100.f);
	const FRotator Rotation = FRotator::ZeroRotator;

	for (int32 X = 0; X < UUBoardManager::GridWidth; X++)
	{
		for (int32 Y = 1; Y < UUBoardManager::GridHeight+1; Y++)
		{
			Location.X = X * BoardManager->CellGap;
			Location.Y = Y * BoardManager->CellGap;

			AABoardCell* Cell = GetWorld()->SpawnActor<AABoardCell>(
				CellActorClass,
				Location,
				Rotation);

			if (!Cell)
			{
				continue;
			}
			Cell->SetActorScale3D(FVector(CellSize, CellSize, CellSize));

			Cell->cellData.Pos = WorldToGrid(Location);

			CellActors.Add(Cell);
		}
	}
}

void AABoardVisualiser::SpawnRefineries(TArray<AActor*>& OutRefineries)
{
	if (!GetWorld() || !RefineryClass)
	{
		return;
	}

	FVector Location(0.f, 0.f, 100.f);
	FRotator Rotation = FRotator::ZeroRotator;

	for (int32 X = 0; X < 2; X++)
	{
		AActor* Refinery = GetWorld()->SpawnActor<AActor>(
			RefineryClass,
			Location,
			Rotation
			);
		
		if (Refinery)
		{
			OutRefineries.Add(Refinery);
		}
	}
}

void AABoardVisualiser::SpawnMotherships(AActor*& OutP0, AActor*& OutP1)
{
	if (!GetWorld() || !RefineryClass)
	{
		return;
	}

	FVector Location(0.f, 0.f, 100.f);
	FRotator Rotation = FRotator::ZeroRotator;

	AActor* MotherShip1 = GetWorld()->SpawnActor<AActor>(
		MothershipClass,
		Location,
		Rotation
		);

	if (MotherShip1)
	{
		OutP0 = MotherShip1;
	}

	AActor* MotherShip2 = GetWorld()->SpawnActor<AActor>(
	MothershipClass,
	Location,
	Rotation
	);

	if (MotherShip2)
	{
		OutP1 = MotherShip2;
	}
}

AActor* AABoardVisualiser::SpawnShip(TSubclassOf<AActor> ship, FIntPoint GridPos)
{
	if (!BoardManager->GetFreeSpawnCells(0).Contains(GridPos))
	{
		return nullptr;
	}
	FVector Location(0.f, 0.f, 500.f);
	const FRotator Rotation = FRotator::ZeroRotator;

	AActor* Ship = GetWorld()->SpawnActor<AActor>(
		ship,
		Location,
		Rotation);

	if (!Ship)
	{
		return nullptr;
	}
	BoardManager->PlaceShip(Ship, GridPos);
	return Ship;
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
