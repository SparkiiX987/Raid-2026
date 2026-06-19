#pragma once

#include "CoreMinimal.h"
#include "../../Upgrades/Upgrade.h"
#include "UpgradeList.generated.h"

USTRUCT()
struct FShipUpgradeList
{
    GENERATED_BODY()

    UPROPERTY()
        TArray<TObjectPtr<UUpgrade>> Upgrades;
};