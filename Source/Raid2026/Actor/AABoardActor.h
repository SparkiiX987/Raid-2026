#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AABoardActor.generated.h"

UCLASS()
class RAID2026_API AABoardActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnShipDamaged, AABoardActor*, Actor, int32, Damage);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
		FOnShipDestroyed, AABoardActor*, Actor);

	UPROPERTY(ReplicatedUsing = OnRep_GridPosition)
		FIntPoint gridPosition;

	int32 maxHealthPoint;
	int32 currentHealthPoint;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int32 ownerPlayer;

	AABoardActor();

	virtual void TakeDamage(int32 amount);

	UPROPERTY(BlueprintAssignable)
	FOnShipDamaged onDamaged;

	UPROPERTY(BlueprintAssignable)
	FOnShipDestroyed onDestroyeds;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentHP()  const;

	UFUNCTION(BlueprintPure)
	int32 GetMaxHP() const;

	UFUNCTION(BlueprintPure)
	bool IsAlive() const;

	UFUNCTION(BlueprintPure)
	FIntPoint GetGridPosition() const;

	UFUNCTION(BlueprintPure)
	int32 GetOwnerID() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetGridPosition(FIntPoint NewPos);

	UFUNCTION(BlueprintNativeEvent)
		void Die();
	virtual void Die_Implementation();

	UFUNCTION()
		void OnRep_GridPosition();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayDamageEffect(int32 DamageAmount);

	UFUNCTION(BlueprintImplementableEvent)
		void OnGridPositionChanged(FIntPoint OldPos, FIntPoint NewPos);
};
