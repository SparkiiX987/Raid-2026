#include "AShip.h"
#include "Net/UnrealNetwork.h"

void AAShip::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAShip, bJustPlayed);
	DOREPLIFETIME(AAShip, bHasActed);
	DOREPLIFETIME(AAShip, bHasMoved);
	DOREPLIFETIME(AAShip, currentSpeed);
}

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
	return currentSpeed;
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
	return GetCurrentSpeed() > 0 && !bJustPlayed;
}

bool AAShip::CanAct() const
{
	return !bHasActed && !bJustPlayed;
}

bool AAShip::CanBePlayed() const
{
	return CanMove() || CanAct();
}

void AAShip::ResetTurnFlags()
{
	bHasMoved = false;
	bHasActed = false;
	bJustPlayed = false;
	currentSpeed = GetEffectiveStats().maxSpeed;
}

void AAShip::OnShipSpawn()
{
	if (true) // check si le vaisseau peut joueur au premier tour
	{
		bJustPlayed = true;
	}

	OnShipSpawnBP();
}

void AAShip::OnMove(int32 Distance)
{
	currentSpeed -= Distance;

	if (!CanMove())
	{
		bHasMoved = true;

		if (!CanAct())
		{
			bJustPlayed = true;
		}
	}

	OnMoveBP();
}

void AAShip::OnAct()
{
	bHasActed = true;

	if (!CanMove())
	{
		bJustPlayed = true;
	}

	OnActBP();
}

void AAShip::TakeDamage(int32 Damage)
{
	Super::TakeDamage(Damage);
}

void AAShip::Die_Implementation()
{
	Super::Die_Implementation();
	
}
