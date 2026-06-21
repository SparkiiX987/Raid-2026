#pragma once

#include "CoreMinimal.h"
#include "AABoardActor.h"
#include "Raid2026/CoreLayer/Cards/UCardData.h"
#include "Raid2026/CoreLayer/Ship/EShipState.h"
#include "AShip.generated.h"

UCLASS()
class RAID2026_API AAShip : public AABoardActor
{
	GENERATED_BODY()

	public:
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UUCardData* CardData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FCardStats RuntimeStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		EShipState State = EShipState::Hidden;

	UFUNCTION(BlueprintCallable)
		FCardStats GetEffectiveStats() const;

	UFUNCTION(BlueprintPure)
		int32 GetFirePower() const;

	UFUNCTION(BlueprintPure)
		int32 GetRadarRange()  const;

	UFUNCTION(BlueprintPure)
		int32 GetMaxSpeed() const;
 
	UFUNCTION(BlueprintPure)
		int32 GetCurrentSpeed() const;

	UFUNCTION(BlueprintPure)
		int32 GetMoveCost() const;
	


	UFUNCTION(BlueprintCallable)
		void Reveal();

	UFUNCTION(BlueprintPure)
		bool IsFaceDown() const;

	UFUNCTION(BlueprintPure)
		bool CanMove() const;

	UFUNCTION(BlueprintPure)
		bool CanAct() const;

	UFUNCTION(BlueprintPure)
		bool CanBePlayed() const;

	UFUNCTION()
		void ApplyUpgrade(UUpgrade* upgrade);

	UFUNCTION()
		void RemoveUpgrade(UUpgrade* upgrade);

	UFUNCTION(BlueprintCallable)
		void ResetTurnFlags();

	UFUNCTION()
		void OnShipSpawn(bool canMoveOnSpawn);

	UFUNCTION(BlueprintImplementableEvent)
		void OnShipSpawnBP();

	UFUNCTION()
		void OnMove(int32 Distance);

	UFUNCTION(BlueprintImplementableEvent)
		void OnMoveBP();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnShipSelectedBP();
	
	UFUNCTION(BlueprintImplementableEvent)
	    void OnShipUnselectedBP();

	UFUNCTION()
		void OnAct();

	UFUNCTION()
		void ServerRemoveUpgrade(UUpgrade* upgrade);

	UFUNCTION(BlueprintImplementableEvent)
		void OnActBP();

	virtual void TakeDamage(int32 Damage) override;

	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
		void PlayRevealAnimation();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		bool bJustPlayed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 currentSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 bonusDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 actions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 actionsPerTurn = 1;

// UFUNCTION(BlueprintImplementableEvent)
//  void SetHighlightState(EHighlightType Type);
};
