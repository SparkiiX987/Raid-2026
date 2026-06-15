#pragma once

#include "CoreMinimal.h"
#include "AABoardActor.h"
#include "Raid2026/CoreLayer/Cards/UCardData.h"
#include "AShip.generated.h"

UCLASS()
class RAID2026_API AAShip : public AABoardActor
{
	GENERATED_BODY()

	public:
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
 UUCardData* CardData;
//
// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//  TArray<UEffect*> ActiveEffects;
//
// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//  EShipState State = EShipState::FaceCachee;

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
		bool IsFaceDown();

	UFUNCTION(BlueprintPure)
		bool CanMove() const;

	UFUNCTION(BlueprintPure)
		bool CanAct() const;

	UFUNCTION(BlueprintPure)
		bool CanBePlayed() const;

	UFUNCTION(BlueprintCallable)
		void ResetTurnFlags();

	UFUNCTION()
		void OnShipSpawn();

	UFUNCTION(BlueprintImplementableEvent)
		void OnShipSpawnBP();

	UFUNCTION()
		void OnMove(int32 Distance);

	UFUNCTION(BlueprintImplementableEvent)
		void OnMoveBP();

	UFUNCTION(BlueprintImplementableEvent)
		void OnShipSelectedBP();

	UFUNCTION()
		void OnAct();

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
		bool bHasMoved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		bool bHasActed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 currentSpeed;

// UFUNCTION(BlueprintImplementableEvent)
//  void SetHighlightState(EHighlightType Type);
};
