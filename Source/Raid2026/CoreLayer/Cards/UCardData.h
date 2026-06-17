#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CardType.h"
#include "CardStats.h"
//#include <Raid2026/Effects/Effect.h>
#include "UCardData.generated.h"

class UEffect;

UCLASS(BlueprintType)
class RAID2026_API UUCardData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCardStats stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECardType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 exemplaire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UStaticMesh> shipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
		TArray<TObjectPtr<UEffect>> Effects;
};
