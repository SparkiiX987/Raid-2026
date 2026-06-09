#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../CoreLayer/Cells/FCell.h"
#include "ABoardCell.generated.h"

UCLASS()
class RAID2026_API AABoardCell : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCell cellData;

	void SetRefineryOwner(int32 owner);

	AABoardCell();
};