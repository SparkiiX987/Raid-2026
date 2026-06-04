#include "ABoardVisualiser.h"

void AABoardVisualiser::SpawnBoard(TArray<AARefinery*>& OutRefineries, AAMotherShip*& OutMothershipP0, AAMotherShip*& OutMothershipP1)
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

			AAShip* Ship = GetWorld()->SpawnActor<AAShip>(
				ShipClass,
				Location,
				Rotation);

			if (!Ship)
			{
				return;
			}
			BoardManager->SetOccupant(FIntPoint(X, Y), Ship);
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

void AABoardVisualiser::SpawnRefineries(TArray<AARefinery*>& OutRefineries)
{
	if (!GetWorld() || !RefineryClass)
	{
		return;
	}

	FVector Location(0.f, 0.f, 100.f);
	FRotator Rotation = FRotator::ZeroRotator;

	for (int32 X = 0; X < 2; X++)
	{
		AARefinery* Refinery = GetWorld()->SpawnActor<AARefinery>(
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

void AABoardVisualiser::SpawnMotherships(AAMotherShip*& OutP0, AAMotherShip*& OutP1)
{
	if (!GetWorld() || !MothershipClass)
	{
		return;
	}

	FVector Location(0.f, 0.f, 100.f);
	FRotator Rotation = FRotator::ZeroRotator;

	AAMotherShip* MotherShip1 = GetWorld()->SpawnActor<AAMotherShip>(
		MothershipClass,
		Location,
		Rotation
		);

	if (MotherShip1)
	{
		OutP0 = MotherShip1;
	}

	AAMotherShip* MotherShip2 = GetWorld()->SpawnActor<AAMotherShip>(
	MothershipClass,
	Location,
	Rotation
	);

	if (MotherShip2)
	{
		OutP1 = MotherShip2;
	}
}

AAShip* AABoardVisualiser::SpawnShip(TSubclassOf<AAShip> ship, FIntPoint GridPos, UUCardData* cardData, int32 PlayerID)
{
	if (!BoardManager->GetFreeSpawnCells(PlayerID).Contains(GridPos) || !TurnManager->PayEssence(TurnManager->GetCurrentPlayer(), cardData->stats.spawnCost))
	{
		return nullptr;
	}
	
	FVector Location(0.f, 0.f, 500.f);
	const FRotator Rotation = FRotator::ZeroRotator;

	AAShip* Ship = GetWorld()->SpawnActor<AAShip>(
		ship,
		Location,
		Rotation);

	if (!Ship)
	{
		return nullptr;
	}
	Ship->CardData = cardData;
	Ship->ownerPlayer = PlayerID;
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
