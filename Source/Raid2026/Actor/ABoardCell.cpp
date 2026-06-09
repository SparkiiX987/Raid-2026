#include "ABoardCell.h"
#include "ARefinery.h"

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
