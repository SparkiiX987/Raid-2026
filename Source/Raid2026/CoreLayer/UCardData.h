#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UCardData.generated.h"

UCLASS(BlueprintType)
class RAID2026_API UUCardData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//FCardStats stats;
	//ECardType type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 exemplaire;
	//TArray<UEffect>;
};
