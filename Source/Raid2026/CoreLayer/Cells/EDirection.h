#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDirections : uint8
{
	North UMETA(DisplayName="North"),
	South UMETA(DisplayName="South"),
	East  UMETA(DisplayName="East"),
	West  UMETA(DisplayName="West")
};