#pragma once

#include "CoreMinimal.h"
#include "../Actor/AShip.h"
#include "../Upgrades/Upgrade.h"
#include "../CoreLayer/Upgrades/UpgradeList.h"
#include "UpgradesManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UUpgradesManager : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION()
		void AddUpgrade(AAShip* upgradeTarget, UUpgrade* upgrade);

	UFUNCTION()
		void RemoveUpgrade(AAShip* ship, UUpgrade* upgrade);

	UPROPERTY()
		TMap<TObjectPtr<AAShip>, FShipUpgradeList> upgradedShips;
};
