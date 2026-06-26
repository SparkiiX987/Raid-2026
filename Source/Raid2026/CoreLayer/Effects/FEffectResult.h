#pragma once

#include "CoreMinimal.h"
#include "FEffectResult.generated.h"

class UUCardData;

USTRUCT(BlueprintType)
struct FEffectResult
{
	GENERATED_BODY()

	UPROPERTY()
		bool bSuccess;
	UPROPERTY()
		bool bNeedsTarget;
	UPROPERTY()
		FString FailReason;

	UPROPERTY()
		int32 IntValue;
	UPROPERTY()
		TArray<TObjectPtr<UUCardData>> AffectedCards;
	UPROPERTY()
		TArray<FIntPoint>  AffectedCells;

	static FEffectResult Success();
	static FEffectResult Fail(const FString& Reason);
	static FEffectResult NeedsTarget();
};
