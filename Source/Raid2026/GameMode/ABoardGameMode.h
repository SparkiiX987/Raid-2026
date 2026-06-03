#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../SubSystem/CombatResolver.h"
#include "../SubSystem/DeckManager.h"
#include "../SubSystem/UTurnManager.h"
#include "../SubSystem/UBoardManager.h"
#include "ABoardGameMode.generated.h"

UCLASS()
class RAID2026_API AABoardGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnyWhere)
		int32 playerCount;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUBoardManager> boardManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUTurnManager> turnManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UDeckManager> deckManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCombatResolver> combatResolverClass;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UUBoardManager> boardManager;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UUTurnManager> turnManager;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UDeckManager> deckManager;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UCombatResolver> combatResolver;

	UFUNCTION(BlueprintPure)
		UUBoardManager* BoardManagerGet() const;

	UFUNCTION(BlueprintPure)
		UUTurnManager* GetTurnManager() const;

	UFUNCTION(BlueprintPure)
		UDeckManager* GetDeckManager() const;

	UFUNCTION(BlueprintPure)
		UCombatResolver* GetCombatResolver() const;

	UFUNCTION(BlueprintCallable)
		void SpawnManagers();
};
