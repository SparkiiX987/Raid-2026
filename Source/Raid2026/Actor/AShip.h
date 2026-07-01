#pragma once

#include "CoreMinimal.h"
#include "AABoardActor.h"
#include "Raid2026/CoreLayer/Cards/UCardData.h"
#include "Raid2026/CoreLayer/Ship/EShipState.h"
#include "../CoreLayer/Effects/EEffectTrigger.h"
#include "AShip.generated.h"

UCLASS()
class RAID2026_API AAShip : public AABoardActor
{
	GENERATED_BODY()

	public:
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UUCardData* CardData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		FCardStats RuntimeStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		EShipState State = EShipState::Hidden;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	int32 protection;

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

	UFUNCTION(BlueprintPure)
		bool IsParalized() const;

	UFUNCTION()
		void StartParalize();

	UFUNCTION()
		void StopParalize();

	void StopShip();
	
	UFUNCTION(BlueprintPure)
	int32 GetFireCost() const;

	UFUNCTION(BlueprintPure)
	bool GetIfIsMovingInDiagonal() const;

	UFUNCTION(BlueprintPure)
	bool GetIfHeCanSpawnShipBesideHim() const;

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
	void ApplyBigCanon();

	UFUNCTION()
	void ApplyMoveInDiagonal();

	UFUNCTION()
	void ApplyCanSpawnShipBesideHim();

	UFUNCTION()
		void ApplyUpgrade(UUpgrade* upgrade);

	UFUNCTION()
		void RemoveUpgrade(UUpgrade* upgrade);

	UFUNCTION(BlueprintCallable)
		void ResetTurnFlags();

	void NotifyEffectTrigger(EEffectTrigger Trigger);

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
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	    void OnShipUnselectedBP();

	UFUNCTION()
		void OnAct();

	UFUNCTION()
		void ServerRemoveUpgrade(UUpgrade* upgrade);

	UFUNCTION(BlueprintImplementableEvent)
		void OnActBP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnGetADiscardCardBP(const TArray<ECardType>& Cards);

	UFUNCTION(BlueprintImplementableEvent)
		void OnParalizeStartBP();

	UFUNCTION(BlueprintImplementableEvent)
		void OnParalizeStopBP();

	virtual void TakeDamage(int32 Damage) override;

	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
		void PlayRevealAnimation();

	UFUNCTION()
		void OnRep_TurnFlags();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_TurnFlags)
		bool bJustPlayed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bPlayDiagonal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bCanSpawnShipBesideHim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_TurnFlags)
		int32 currentSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 bonusDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_TurnFlags)
		int32 actions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 actionsPerTurn = 1;

	UPROPERTY(BlueprintReadOnly, Replicated)
		bool bIsParalized = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	int32 fireCost = 1;

// UFUNCTION(BlueprintImplementableEvent)
//  void SetHighlightState(EHighlightType Type);
};
