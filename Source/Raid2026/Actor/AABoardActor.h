#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Raid2026/Upgrades/Upgrade.h>
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 maxHealthPoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		int32 currentHealthPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int32 bonusHealth;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	int32 EffectMaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	int32 EffectCurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bHaveEffectHealthActive;
	

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

	void SetHealthPoint(int32 health);

	UFUNCTION(BlueprintPure)
	FIntPoint GetGridPosition() const;

	UFUNCTION(BlueprintPure)
	int32 GetOwnerID() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetGridPosition(FIntPoint NewPos);

	UFUNCTION(BlueprintCallable)
	void Heal(int32 amount);

	UFUNCTION(BlueprintNativeEvent)
		void Die();
	virtual void Die_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
		void OnActorDamagedBP();

	UFUNCTION()
		void OnRep_GridPosition();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayDamageEffect(int32 DamageAmount);

	UFUNCTION(BlueprintImplementableEvent)
		void OnGridPositionChanged(FIntPoint OldPos, FIntPoint NewPos);

	UPROPERTY()
		TArray<TObjectPtr<UUpgrade>> upgrades;
};
