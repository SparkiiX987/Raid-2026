#pragma once

#include "CoreMinimal.h"
#include "FireResult.generated.h"

USTRUCT(BlueprintType)
struct FFireResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		bool bHit = false;

	UPROPERTY(BlueprintReadOnly)
		bool  bShipDestroyed = false;

	UPROPERTY(BlueprintReadOnly)
		int32 DamageDealt = 0;

	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<AActor> HitShip; // TODO changer en AShipActor quand il existera
};