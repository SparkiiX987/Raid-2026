#include "AMotherShip.h"

bool AAMotherShip::AddRDCard(UUCardData* Card)
{
	if (HasFreeRDSlot())
	{
		RDCards.Add(Card);
		OnRDSlotsChangedBP();
		return true;
	}
	return false;
}

bool AAMotherShip::RemoveRDCard(int32 SlotIndex)
{
	RDCards.RemoveAt(SlotIndex);
	OnRDSlotsChangedBP();
	return true;
}

bool AAMotherShip::HasFreeRDSlot() const
{
	if (GetRDSlotCount() < MaxRDSlots)
	{
		return true;
	}
	return false;
}

int32 AAMotherShip::GetRDSlotCount() const
{
	return RDCards.Num();
}

void AAMotherShip::Die_Implementation()
{
	Super::Die_Implementation();
	OnMothershipDefeated.Broadcast(this);
	PlayDestructionSequence();
}
