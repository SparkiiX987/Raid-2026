#include "AShip.h"
#include "../Upgrades/Upgrade.h"
#include "Net/UnrealNetwork.h"
#include <Raid2026/GameMode/ABoardGameMode.h>

void AAShip::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAShip, bJustPlayed);
	DOREPLIFETIME(AAShip, currentSpeed);
	DOREPLIFETIME(AAShip, actions);
	DOREPLIFETIME(AAShip, actionsPerTurn);
}

FCardStats AAShip::GetEffectiveStats() const
{
	return CardData->stats;
}

int32 AAShip::GetFirePower() const
{
	return GetEffectiveStats().firePower + bonusDamage;
}

int32 AAShip::GetRadarRange() const
{
	return GetEffectiveStats().radar;
}

int32 AAShip::GetMaxSpeed() const
{
	if (IsFaceDown())
	{
		return 1;
	}
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
	State = EShipState::Visible;
	if (CanMove())
	{
		currentSpeed = GetMaxSpeed();
	}
	PlayRevealAnimation();
}

bool AAShip::IsFaceDown() const
{
	return State == EShipState::Hidden;
}

bool AAShip::CanMove() const
{
	return GetCurrentSpeed() > 0 && !bJustPlayed;
}

bool AAShip::CanAct() const
{
	return actions > 0 && !bJustPlayed;
}

bool AAShip::CanBePlayed() const
{
	return CanMove() || CanAct();
}

void AAShip::TakeDamage(int32 amount)
{
	int32 remainingDamages = amount;

	TArray<UUpgrade*> shieldsToDestroy;

	for (UUpgrade* upgrade : upgrades)
	{
		if (remainingDamages <= 0)
		{
			break;
		}

		if (!upgrade || upgrade->EffectiveShieldHealth <= 0)
		{
			continue;
		}

		if (upgrade->EffectiveShieldHealth > remainingDamages)
		{
			upgrade->EffectiveShieldHealth -= remainingDamages;
			bonusHealth -= remainingDamages;
			remainingDamages = 0;
		}
		else
		{
			remainingDamages -= upgrade->EffectiveShieldHealth;
			bonusHealth -= upgrade->EffectiveShieldHealth;
			upgrade->EffectiveShieldHealth = 0;
			shieldsToDestroy.Add(upgrade);
		}
	}

	for (UUpgrade* upgrade : shieldsToDestroy)
	{
		ServerRemoveUpgrade(upgrade);
	}

	if (remainingDamages > 0)
	{
		currentHealthPoint -= remainingDamages;
	}

	if (IsFaceDown())
	{
		Reveal();
	}
	Super::TakeDamage(Damage);

	OnActorDamagedBP();
}

void AAShip::ApplyUpgrade(UUpgrade* upgrade)
{
	upgrades.Add(upgrade);

	int32 shield = maxHealthPoint * upgrade->ShieldHealthRatio;
	bonusHealth += shield;
	upgrade->EffectiveShieldHealth = shield;
	bonusDamage += upgrade->DamageBonus;
	actionsPerTurn += upgrade->BonusActions;
	CardData->stats.maxSpeed += upgrade->BonusMovespeed;
}

void AAShip::RemoveUpgrade(UUpgrade* upgrade)
{
	upgrades.Remove(upgrade);

	bonusHealth = bonusHealth - upgrade->EffectiveShieldHealth;
	
	bonusDamage = (bonusDamage - upgrade->DamageBonus <= 0)
		? 1 : bonusDamage - upgrade->DamageBonus;

	actionsPerTurn = (actionsPerTurn - upgrade->BonusActions <= 0)
		? 1 : actionsPerTurn - upgrade->BonusActions;

	CardData->stats.maxSpeed = (CardData->stats.maxSpeed - upgrade->BonusMovespeed <= 0)
		? 1 : CardData->stats.maxSpeed - upgrade->BonusMovespeed;
}

void AAShip::ResetTurnFlags()
{
	actions = actionsPerTurn;
	bJustPlayed = false;
	currentSpeed = GetMaxSpeed();
}

void AAShip::OnShipSpawn(bool canMoveOnSpawn)
{
	bJustPlayed = canMoveOnSpawn;

	OnShipSpawnBP();
}

void AAShip::OnMove(int32 Distance)
{
	currentSpeed -= Distance;

	if (!CanMove())
	{
		if (!CanAct())
		{
			bJustPlayed = true;
		}
	}

	OnMoveBP();
}

void AAShip::OnAct()
{
	if (IsFaceDown())
	{
		Reveal();
	}

	actions--;
	
	if (!CanAct())
	{
		if (!CanMove())
		{
			bJustPlayed = true;
		}
	}

	OnActBP();
}

void AAShip::ServerRemoveUpgrade(UUpgrade* upgrade)
{
	AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
	if (!GM) return;

	GM->HandleRemoveUpgrade(upgrade,this);
}

void AAShip::Die_Implementation()
{
	Super::Die_Implementation();
	
}
