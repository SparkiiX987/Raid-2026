#include "UpgradesManager.h"

void UUpgradesManager::AddUpgrade(AAShip* upgradeTarget, UUpgrade* upgrade)
{
	if (!upgrade || !IsValid(upgradeTarget))
		return;

	upgradeTarget->ApplyUpgrade(upgrade);
	upgradedShips.FindOrAdd(upgradeTarget).Upgrades.Add(upgrade);
}

void UUpgradesManager::RemoveUpgrade(AAShip* ship, UUpgrade* upgrade)
{
	if (!upgrade || !IsValid(ship))
		return;

	ship->RemoveUpgrade(upgrade);

	if (FShipUpgradeList* List = upgradedShips.Find(ship))
	{
		List->Upgrades.Remove(upgrade);
		if (List->Upgrades.Num() == 0)
		{
			upgradedShips.Remove(ship);
		}
	}
}
