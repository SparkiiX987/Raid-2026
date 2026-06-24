#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DeckListSave.generated.h"

UCLASS()
class UDeckListSave : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> DeckNames;
};
