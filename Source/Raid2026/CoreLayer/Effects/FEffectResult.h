#pragma once

#include "CoreMinimal.h"
#include "FEffectResult.generated.h"

USTRUCT(BlueprintType)
struct FEffectResult
{
	GENERATED_BODY()

	bool bSuccess;
	bool bNeedsTarget;
	FString FailReason;

	int32 IntValue;
	//TArray<UCardData*> AffectedCards;
	TArray<FIntPoint>  AffectedCells;

	static FEffectResult Success();
	static FEffectResult Fail(const FString& Reason);
	static FEffectResult NeedsTarget();
};
