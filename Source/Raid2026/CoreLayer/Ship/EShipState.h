#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EShipState : uint8
{
	Hidden UMETA(DisplayName="Hidden"),
	Visible UMETA(DisplayName="Visible")
};