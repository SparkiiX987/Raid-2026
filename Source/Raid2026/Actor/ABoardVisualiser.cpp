#include "ABoardVisualiser.h"

#include <SceneExport.h>

#include "Raid2026/SubSystem/UBoardManager.h"

void AABoardVisualiser::BeginPlay()
{
	Super::BeginPlay();
}

void AABoardVisualiser::SpawnBoard(TArray<AActor*>& OutRefineries, AActor*& OutMothershipP0, AActor*& OutMothershipP1)
{
	SpawnCellActors();
	SpawnRefineries(OutRefineries);
	SpawnMotherships(OutMothershipP0, OutMothershipP1);
}

void AABoardVisualiser::DebugSpawnShipInAllCells()
{
}

AABoardCell* AABoardVisualiser::GetCellActor(FIntPoint GridPos) const
{
	return nullptr;
}

FVector AABoardVisualiser::GridToWorld(FIntPoint GridPos) const
{
	return FVector();
}

FIntPoint AABoardVisualiser::WorldToGrid(FVector WorldPos) const
{
	return FIntPoint();
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
		for (int32 Y = 0; Y < UUBoardManager::GridHeight; Y++)
		{
			Location.X = X * CellGap;
			Location.Y = Y * CellGap;

			AABoardCell* Cell = GetWorld()->SpawnActor<AABoardCell>(
				CellActorClass,
				Location,
				Rotation);

			if (!Cell)
			{
				continue;
			}

			Cell->cellData.Pos = FIntPoint(X, Y);

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

void AABoardVisualiser::SpawnShip(TSubclassOf<AActor>)
{
}

int32 AABoardVisualiser::CellIndex(int32 X, int32 Y) const
{
	return 0;
}
