#pragma once

#include "CoreMinimal.h"
#include "AABoardActor.h"
#include "Raid2026/CoreLayer/Cards/UCardData.h"
#include "AMotherShip.generated.h"

UCLASS()
class RAID2026_API AAMotherShip : public AABoardActor
{
	GENERATED_BODY()

public:
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMothershipDefeated,AAMotherShip*, Mothership);

	AAMotherShip()
	{
		maxHealthPoint = 20;
		currentHealthPoint = 20;
	}
	
	static constexpr int32 MaxRDSlots = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UUCardData*> RDCards;

	UFUNCTION(BlueprintCallable)
		bool AddRDCard(UUCardData* Card);

	UFUNCTION(BlueprintCallable)
		bool RemoveRDCard(int32 SlotIndex);

	UFUNCTION(BlueprintPure)
		bool HasFreeRDSlot() const;

	UFUNCTION(BlueprintPure)
		int32 GetRDSlotCount() const;

	UPROPERTY(BlueprintAssignable)
		FOnMothershipDefeated OnMothershipDefeated;

	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
		void PlayDestructionSequence();

	UFUNCTION(BlueprintImplementableEvent)
		void OnRDSlotsChangedBP();
	
};
