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

int32 AAShip::GetMaxSpeed() const
{
	return GetEffectiveStats().maxSpeed;
}

int32 AAShip::GetCurrentSpeed() const
{
	return GetEffectiveStats().currentSpeed;
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
	return GetCurrentSpeed() > 0 && bJustPlayed;
}

bool AAShip::CanAct() const
{
	return bHasActed;
}

bool AAShip::CanBePlayed() const
{
	return !bJustPlayed && (!bHasMoved || !bHasActed);
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
