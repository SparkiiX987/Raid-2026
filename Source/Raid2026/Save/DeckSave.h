#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DeckSave.generated.h"

UCLASS()
class RAID2026_API UDeckSave : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TMap<int32, int32> SavedDeck;
};