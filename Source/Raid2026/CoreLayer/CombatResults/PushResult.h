#pragma once

#include "CoreMinimal.h"
#include "Collision.h"
#include "PushResult.generated.h"

USTRUCT(BlueprintType)
struct FPushResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		bool bMoved = false;

	UPROPERTY(BlueprintReadOnly)
		FIntPoint FinalCell = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly)
		TArray<FCollision> Collisions;
};