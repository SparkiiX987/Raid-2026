#pragma once

#include "CoreMinimal.h"
#include "../CoreLayer/CombatResults/PushResult.h"
#include "../CoreLayer/CombatResults/FireResult.h"
#include "../CoreLayer/CombatResults/Collision.h"
#include "../CoreLayer/Cells/EDirection.h"
#include "UBoardManager.h"
#include "CombatResolver.generated.h"

UCLASS()
class RAID2026_API UCombatResolver : public UObject
{
	GENERATED_BODY()
	
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnShipDamaged, AActor*, Ship, int32, Damage); // TODO changer en AShipActor quand il existera

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnShipDestroyed, AActor*, Ship); // TODO changer en AShipActor quand il existera

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnCollision, AActor*, ShipA, AActor*, ShipB); // TODO changer en AShipActor quand il existera

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnMothershipDamaged, AActor*, Mothership, int32, Damage); // TODO changer en AMotherShipActor quand il existera

public:
    UPROPERTY(BlueprintAssignable)
        FOnShipDamaged OnShipDamaged;

    UPROPERTY(BlueprintAssignable)
        FOnShipDestroyed OnShipDestroyed;

    UPROPERTY(BlueprintAssignable)
        FOnCollision OnCollision;

    UPROPERTY(BlueprintAssignable)
        FOnMothershipDamaged OnMotherShipDamaged;

    UFUNCTION(BlueprintCallable)
        FFireResult ResolveFire(AActor* Shooter, FIntPoint TargetCell);// TODO changer en AShipActor quand il existera

    UFUNCTION(BlueprintCallable)
        void ApplyDamageToShip(AActor* Target, int32 Damage); // TODO changer en AShipActor quand il existera

    UFUNCTION(BlueprintCallable)
        void ApplyDamageToMothership(AActor* Target, int32 Damage); // TODO changer en AShipActor quand il existera

    UFUNCTION(BlueprintCallable)
        FPushResult ApplyPush(AActor* Ship, EDirections Direction, int32 MaxChainDepth = 8); // TODO changer en AShipActor quand il existera

    UFUNCTION(BlueprintCallable, BlueprintPure)
        bool HasLineOfSight(AActor* Shooter, FIntPoint TargetCell) const; // TODO changer en AShipActor quand il existera

    UFUNCTION(BlueprintCallable)
        TArray<FIntPoint> GetValidFireTargets(AActor* Shooter) const; // TODO changer en AShipActor quand il existera

    void DestroyShip(AActor* Ship); // TODO changer en AShipActor quand il existera

    void ResolveCollision(AActor* ShipA, AActor* ShipB, FPushResult& OutResult, int32 RemainingDepth); // TODO changer en AShipActor quand il existera

    FIntPoint GetCellBehind(AActor* Ship, EDirections PushDirection) const; // TODO changer en AShipActor quand il existera

    UPROPERTY(EditAnywhere)
    TObjectPtr<UUBoardManager> Board;

    static const TArray<FIntPoint> AllDirections;
};
