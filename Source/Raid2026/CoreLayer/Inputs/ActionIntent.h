#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EActionIntent : uint8
{
	NONE UMETA(DisplayName = "None"),
	MOVE UMETA(DisplayName = "Move"),
	FIRE UMETA(DisplayName = "Fire"),
	PLAYCARD UMETA(DisplayName = "play card")
};
