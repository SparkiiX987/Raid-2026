#pragma once

#include "CoreMinimal.h"
#include "../CoreLayer/CombatResults/PushResult.h"
#include "../CoreLayer/CombatResults/FireResult.h"
#include "../CoreLayer/CombatResults/Collision.h"
#include "../CoreLayer/Cells/EDirection.h"
#include "../Actor/AShip.h"
#include "../Actor/AMotherShip.h"
#include "UBoardManager.h"
#include "CombatResolver.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID2026_API UCombatResolver : public UObject
{
	GENERATED_BODY()
	
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnShipDamaged, AAShip*, Ship, int32, Damage);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnShipDestroyed, AAShip*, Ship);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnCollision, AAShip*, ShipA, AAShip*, ShipB);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnMothershipDamaged, AAMotherShip*, Mothership, int32, Damage);

public:
    UPROPERTY(BlueprintAssignable)
        FOnShipDamaged OnShipDamaged;

    UPROPERTY(BlueprintAssignable)
        FOnShipDestroyed OnShipDestroyed;

    UPROPERTY(BlueprintAssignable)
        FOnCollision OnCollision;

    UPROPERTY(BlueprintAssignable)
        FOnMothershipDamaged OnMothershipDamaged;

    UFUNCTION(BlueprintCallable)
        FFireResult ResolveFire(AAShip* Shooter, FIntPoint TargetCell);

    UFUNCTION(BlueprintCallable)
        void ApplyDamageToShip(AAShip* Target, int32 Damage);

    UFUNCTION(BlueprintCallable)
    void ApplyDamageToMothership(AAMotherShip* Target, int32 Damage);

    UFUNCTION(BlueprintCallable)
        FPushResult ApplyPush(AAShip* Ship, EDirections Direction, int32 MaxChainDepth = 8);

    UFUNCTION(BlueprintCallable)
        TArray<FIntPoint> GetValidFireTargets(AAShip* Shooter) const;

    void DestroyShip(AAShip* Ship);

    void ResolveCollision(AAShip* ShipA, AAShip* ShipB, FPushResult& OutResult, int32 RemainingDepth);

    FIntPoint GetCellBehind(AAShip* Ship, EDirections PushDirection) const;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UUBoardManager> Board;

    static const TArray<FIntPoint> AllDirections;

    FIntPoint DirectionToIntPoint(EDirections dir) const;

    EDirections IntPointToDirection(FIntPoint Delta);
};
