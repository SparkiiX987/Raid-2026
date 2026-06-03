#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECellType : uint8
{
	Normal     UMETA(DisplayName="Normal"),
	SpawnP0    UMETA(DisplayName="Spawn P0"),
	SpawnP1    UMETA(DisplayName="Spawn P1"),
	Refinery   UMETA(DisplayName="Refinery")
};
