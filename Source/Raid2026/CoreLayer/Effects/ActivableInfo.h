#pragma once
#include "CoreMinimal.h"
#include "EffectTargetKind.h"
#include "ActivableInfo.generated.h"

USTRUCT(BlueprintType)
struct FActivatableEffectInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) 
        int32 Index = -1;

    UPROPERTY(BlueprintReadOnly) 
        FText DisplayName;

    UPROPERTY(BlueprintReadOnly) 
        FText Description;

    UPROPERTY(BlueprintReadOnly) 
        int32 EssenceCost = 0;

    UPROPERTY(BlueprintReadOnly) 
        bool  bNeedsTarget = false;

    UPROPERTY(BlueprintReadOnly)
        EEffectTargetKind TargetKind = EEffectTargetKind::None;
};