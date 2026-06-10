#pragma once

#include "CoreMinimal.h"
#include "../../Actor/AShip.h"
#include "FireResult.generated.h"

USTRUCT(BlueprintType)
struct FFireResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		bool bHit = false;

	UPROPERTY(BlueprintReadOnly)
		bool bMothershipHit = false;

	UPROPERTY(BlueprintReadOnly)
		bool  bShipDestroyed = false;

	UPROPERTY(BlueprintReadOnly)
		int32 DamageDealt = 0;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AAShip> HitShip;
};