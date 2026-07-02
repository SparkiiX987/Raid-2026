#include "AShip.h"
#include "../Upgrades/Upgrade.h"
#include "Net/UnrealNetwork.h"
#include <Raid2026/GameMode/ABoardGameMode.h>

void AAShip::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAShip, bJustPlayed);
	DOREPLIFETIME(AAShip, bPlayDiagonal);
	DOREPLIFETIME(AAShip, currentSpeed);
	DOREPLIFETIME(AAShip, actions);
	DOREPLIFETIME(AAShip, actionsPerTurn);
	DOREPLIFETIME(AAShip, State);
	DOREPLIFETIME(AAShip, bonusDamage);
	DOREPLIFETIME(AAShip, RuntimeStats);
	DOREPLIFETIME(AAShip, fireCost);
	DOREPLIFETIME(AAShip, bIsParalized);
}

FCardStats AAShip::GetEffectiveStats() const
{
	return RuntimeStats;
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

bool AAShip::IsParalized() const
{
	return bIsParalized;
}

void AAShip::StartParalize()
{
	if (GEngine)
	{
		FString text = FString::Printf(TEXT("Paralized"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, text);
	}

	bIsParalized = true;
	bJustPlayed = true;
	OnParalizeStartBP();
}

void AAShip::StopParalize()
{
	if (GEngine)
	{
		FString text = FString::Printf(TEXT("Stop paralize"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, text);
	}
	bIsParalized = false;
	//bJustPlayed = true;
	OnParalizeStopBP();
}

void AAShip::StopShip()
{
	bJustPlayed = true;
}

int32 AAShip::GetFireCost() const
{
	return fireCost;
}

bool AAShip::GetIfIsMovingInDiagonal() const
{
	return bPlayDiagonal;
}

bool AAShip::GetIfHeCanSpawnShipBesideHim() const
{
	return bCanSpawnShipBesideHim;
}

void AAShip::Reveal()
{
	State = EShipState::Visible;
	NotifyEffectTrigger(EEffectTrigger::OnReveal);
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
	if (GEngine)
	{
		FString text = FString::Printf(TEXT("Can Move : %d, Can Act : %d, Just Played : %d"), CanMove(), CanAct(), bJustPlayed);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
	}
	return CanMove() || CanAct();
}

void AAShip::ApplyBigCanon()
{
	actionsPerTurn++;
	fireCost++;
}

void AAShip::ApplyMoveInDiagonal()
{
	bPlayDiagonal = true;
}

void AAShip::ApplyCanSpawnShipBesideHim()
{
	bCanSpawnShipBesideHim = true;
}

void AAShip::TakeDamage(int32 amount)
{
	int32 remainingDamages = amount;

	if (protection > 0)
	{
		remainingDamages -= protection;
		protection -= amount;
	}

	if (bHaveEffectHealthActive && EffectCurrentHealth > 0)
	{
		int32 CurrentHealth = EffectCurrentHealth;
		
		EffectCurrentHealth -= remainingDamages;
		remainingDamages -= CurrentHealth;
		if (EffectCurrentHealth < 0)
		{
			EffectCurrentHealth = 0;
		}
	}
	
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

	OnActorDamagedBP();
	NotifyEffectTrigger(EEffectTrigger::OnDamageTaken);
}

void AAShip::ApplyUpgrade(UUpgrade* upgrade)
{
	upgrades.Add(upgrade);

	int32 shield = maxHealthPoint * upgrade->ShieldHealthRatio;
	bonusHealth += shield;
	upgrade->EffectiveShieldHealth = shield;
	bonusDamage += upgrade->DamageBonus;
	actionsPerTurn += upgrade->BonusActions;
	RuntimeStats.maxSpeed += upgrade->BonusMovespeed;
}

void AAShip::RemoveUpgrade(UUpgrade* upgrade)
{
	upgrades.Remove(upgrade);

	bonusHealth -= upgrade->EffectiveShieldHealth;

	bonusDamage = FMath::Max(1, bonusDamage - upgrade->DamageBonus);
	actionsPerTurn = FMath::Max(1, actionsPerTurn - upgrade->BonusActions);
	RuntimeStats.maxSpeed = FMath::Max(1, RuntimeStats.maxSpeed - upgrade->BonusMovespeed);
}

void AAShip::ResetTurnFlags()
{
	if (GEngine)
	{
		FString text = FString::Printf(TEXT("Is Paralized : %d"), IsParalized());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, text);
	}

	if (IsParalized())
	{
		StopParalize();
		return;
	}

	if (GEngine)
	{
		FString text = FString::Printf(TEXT("Set can move for ship : "));
		text += GetName();
		text += " of player ";
		text += FString::FromInt(ownerPlayer);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, text);
	}

	actions = actionsPerTurn;
	bJustPlayed = false;
	currentSpeed = GetMaxSpeed();
	CanBePlayed();
}

void AAShip::NotifyEffectTrigger(EEffectTrigger Trigger)
{
	UWorld* World = GetWorld();
	AABoardGameMode* GM = World ? World->GetAuthGameMode<AABoardGameMode>() : nullptr;
	if (!GM || !GM->effectManager) return;

	GM->effectManager->NotifyShipEvent(this, Trigger, FEffectContext());
}

void AAShip::OnShipSpawn(bool canMoveOnSpawn)
{
	if (canMoveOnSpawn)
	{
		bJustPlayed = false;
		currentSpeed = GetMaxSpeed();
		actions = actionsPerTurn;
	}


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

void AAShip::OnRep_TurnFlags()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,
			FString::Printf(TEXT("OnRep_TurnFlags: %s | bJustPlayed=%d | speed=%d | actions=%d"),
				*GetName(), bJustPlayed, currentSpeed, actions));
	}
}
