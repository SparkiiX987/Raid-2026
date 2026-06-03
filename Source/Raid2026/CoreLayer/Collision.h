#pragma once

#include "CoreMinimal.h"
#include "Collision.generated.h"

USTRUCT(BlueprintType)
struct FCollision
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<AActor> ShipA; // TODO changer en AShipActor quand il existera

	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<AActor> ShipB; // TODO changer en AShipActor quand il existera

	UPROPERTY(BlueprintReadOnly)
		int32 DamageEach = 2;
};