#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECardType : uint8
{
	SHIP     UMETA(DisplayName = "Ship"),
	SABOTAGE    UMETA(DisplayName = "Sabotage"),
	UPDRAGE    UMETA(DisplayName = "Upgrade"),
	EXPERT    UMETA(DisplayName = "Expert")
};
