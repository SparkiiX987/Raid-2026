#pragma once
#include "CoreMinimal.h"
#include "EffectTargetKind.generated.h"

UENUM(BlueprintType)
enum class EEffectTargetKind : uint8
{
    None UMETA(DisplayName = "None"),
    Ship UMETA(DisplayName = "Ship"),
    Cell UMETA(DisplayName = "Cell")
};