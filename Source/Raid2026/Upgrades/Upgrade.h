#pragma once

#include "CoreMinimal.h"
#include "Upgrade.generated.h"

class UUCardData;

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UUpgrade : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUCardData> SourceCard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float ShieldHealthRatio;

	UPROPERTY(BlueprintReadOnly)
		int32 EffectiveShieldHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 DamageBonus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 BonusActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 BonusMovespeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<int32> targetedShipClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;
};
