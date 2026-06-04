#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Raid2026/Actor/ABoardCell.h"
#include "Raid2026/Actor/ABoardVisualiser.h"
#include "Raid2026/Actor/AShip.h"
#include "BoardPlayerController.generated.h"

UCLASS()
class RAID2026_API ABoardPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUBoardManager* BoardManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AABoardVisualiser* BoardVisualiser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUTurnManager* UUTurnManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AAShip* SelectedShip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUCardData* CardData;

	UFUNCTION(BlueprintCallable)
	void ClickOnShip(AAShip* selectShip);
	
	UFUNCTION(BlueprintCallable)
	void ClickOnCell(AABoardCell* selectedCell);
	
};
