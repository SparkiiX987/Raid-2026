#include "AShip.h"


FCardStats AAShip::GetEffectiveStats() const
{
	return CardData->stats;
}

int32 AAShip::GetFirePower() const
{
	return GetEffectiveStats().firePower;
}

int32 AAShip::GetRadarRange() const
{
	return GetEffectiveStats().radar;
}

int32 AAShip::GetSpeed() const
{
	return GetEffectiveStats().speed;
}

int32 AAShip::GetMoveCost() const
{
	return GetEffectiveStats().moveCost;
}

void AAShip::Reveal()
{
	//State = Face
	PlayRevealAnimation();
}

bool AAShip::IsFaceDown()
{
	//return State == EShipState::FaceCachee;
	return false;
}

bool AAShip::CanMove() const
{
	return bHasMoved;
}

bool AAShip::CanAct() const
{
	return bHasActed;
}

void AAShip::ResetTurnFlags()
{
	bHasMoved = false;
	bHasActed = false;
	bJustPlayed = false;
}

void AAShip::TakeDamage(int32 Damage)
{
	Super::TakeDamage(Damage);
}

void AAShip::Die_Implementation()
{
	Super::Die_Implementation();
	
}
