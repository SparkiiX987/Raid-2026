#pragma once

#include "CoreMinimal.h"
#include "AABoardActor.h"
#include "ARefinery.generated.h"

UCLASS()
class RAID2026_API AARefinery : public AABoardActor
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRefineryCaptured,
		AARefinery*, Refinery, int32, NewOwnerPlayerID);

AARefinery()
{
	maxHealthPoint = 0;
	currentHealthPoint = 0;
}

UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ControllerPlayerID = -1;

UFUNCTION(BlueprintPure)
	bool IsNeutral() const;

UFUNCTION(BlueprintPure)
	bool IsControlledBy(int32 PlayerID) const;

 UFUNCTION(BlueprintCallable)
	void Capture(int32 NewControllerID);

UFUNCTION(BlueprintCallable)
	void Neutralize();

UPROPERTY(BlueprintAssignable)
	FOnRefineryCaptured OnCaptured;

UFUNCTION(BlueprintImplementableEvent)
	void UpdateControlIndicator(int32 PlayerID);

UFUNCTION(BlueprintImplementableEvent)
	void PlayCaptureEffect(int32 NewOwnerID);
};
