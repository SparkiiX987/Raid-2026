#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CardType.h"
#include "CardStats.h"
#include "UCardData.generated.h"

class UEffect;

UCLASS(BlueprintType)
class RAID2026_API UUCardData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString cardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 cardId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "type==ECardType::SHIP"))
		FCardStats stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 playCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECardType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "type==ECardType::SHIP"))
		TObjectPtr<UStaticMesh> shipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
		TArray<TObjectPtr<UEffect>> Effects;
};
