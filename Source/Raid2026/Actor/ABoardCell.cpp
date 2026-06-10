#include "ABoardCell.h"
#include "ARefinery.h"
#include "Net/UnrealNetwork.h"

void AABoardCell::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AABoardCell, cellData);
}

void AABoardCell::SetRefineryOwner(int32 owner)
{
	AARefinery* refinery = Cast<AARefinery>(cellData.Occupant);

	if (!IsValid(refinery))	return;

	refinery->Capture(owner);
}

AABoardCell::AABoardCell()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}
