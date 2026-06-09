#include "CombatResolver.h"

const TArray<FIntPoint> UCombatResolver::AllDirections =
{
	FIntPoint(0,  1),  // Nord
	FIntPoint(0, -1),  // Sud
	FIntPoint(-1,  0),  // Ouest
	FIntPoint(1,  0)  // Est
};

FFireResult UCombatResolver::ResolveFire(AAShip* Shooter, FIntPoint TargetCell)
{
    FFireResult Result;

    if (!Shooter)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("ResolveFire: Shooter est null"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return Result;
    }

    if (!Board) return Result;

    if (!Board->IsLineOfSight(Shooter->GetGridPosition(), TargetCell))
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("ResolveFire: Pas de LdV vers (%d,%d)"),
                TargetCell.X, TargetCell.Y);

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return Result;
    }

    if (Shooter->IsFaceDown())
    {
        Shooter->Reveal();
    }

    AAShip* TargetShip = Board->GetShipAt(TargetCell);

    if (TargetShip)
    {
        const int32 Damage = Shooter->GetFirePower();
        Result.bHit = true;
        Result.DamageDealt = Damage;
        Result.HitShip = TargetShip;

        ApplyDamageToShip(TargetShip, Damage);

        Result.bShipDestroyed = !IsValid(TargetShip);
        return Result;
    }

    AAMotherShip* TargetMothership = Board->GetMothershipAt(TargetCell);
    if (TargetMothership)
    {
        const int32 Damage = Shooter->GetFirePower();
        Result.bHit = true;
        Result.DamageDealt = Damage;

        ApplyDamageToMothership(TargetMothership, Damage);
        return Result;
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ResolveFire: Cible vide en (%d,%d)"),
            TargetCell.X, TargetCell.Y);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
    return Result;
}

void UCombatResolver::ApplyDamageToShip(AAShip* Target, int32 Damage)
{
    if (!Target || Damage <= 0) return;

    Target->TakeDamage(Damage);
    OnShipDamaged.Broadcast(Target, Damage);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ApplyDamage: %s reçoit %d dégâts → résistance = %d"),
            *Target->GetName(), Damage, Target->GetEffectiveStats().resistance);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    if (Target->GetEffectiveStats().resistance <= 0)
    {
        DestroyShip(Target);
    }
}

void UCombatResolver::ApplyDamageToMothership(AAMotherShip* Target, int32 Damage)
{
    if (!Target ||Damage <= 0) return;

    Target->TakeDamage(Damage);
    OnMothershipDamaged.Broadcast(Target, Damage);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ApplyDamage: Vaisseau-mère reçoit %d dégâts → PV = %d"),
            Damage, Target->GetCurrentHP());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

FPushResult UCombatResolver::ApplyPush(AAShip* Ship, EDirections Direction, int32 MaxChainDepth)
{
    FPushResult Result;

    if (!Ship || MaxChainDepth <= 0)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("ApplyPush: Ship null ou profondeur max atteinte"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return Result;
    }

    if (!Board) return Result;

    const FIntPoint Behind = GetCellBehind(Ship, Direction);

    if (Board->IsValidCell(Behind) && !Board->IsCellOccupied(Behind))
    {
        Board->MoveShipTo(Ship, Behind, Ship->ownerPlayer);
        Result.bMoved = true;
        Result.FinalCell = Behind;
        return Result;
    }

    AAShip* CollidingShip = Board->GetShipAt(Behind);

    if (CollidingShip)
    {
        ResolveCollision(Ship, CollidingShip, Result, MaxChainDepth - 1);
        Result.bMoved = false;
        return Result;
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ApplyPush: %s bloqué au bord du plateau"),
        *Ship->GetName());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
    
    return Result;
}

TArray<FIntPoint> UCombatResolver::GetValidFireTargets(AAShip* Shooter) const
{
    TArray<FIntPoint> ValidTargets;
    if (!Shooter) return ValidTargets;

    if (!Board)   return ValidTargets;

    const FIntPoint Origin = Shooter->GetGridPosition();
    const int32     Radar = Shooter->GetRadarRange();

    const TArray<FIntPoint> Orthogonals = 
    {
        { 0,  1}, { 0, -1}, {-1,  0}, { 1,  0}
    };

    for (const FIntPoint& Dir : Orthogonals)
    {
        FIntPoint Current = Origin + Dir;
        int32     Steps = 0;

        while (Steps < Radar && Board->IsValidCell(Current))
        {
            AAShip* OccupantShip = Board->GetShipAt(Current);

            if (OccupantShip)
            {
                if (OccupantShip->GetOwnerID() != Shooter->GetOwnerID()
                    && !OccupantShip->IsFaceDown())
                {
                    ValidTargets.Add(Current);
                }
                break;
            }

            if (Board->IsMothershipCell(Current, Shooter->GetOwnerID()))
            {
                ValidTargets.Add(Current);
                break;
            }

            Current += Dir;
            ++Steps;
        }
    }

    return ValidTargets;
}

void UCombatResolver::DestroyShip(AAShip* Ship)
{
    if (!Ship) return;

    if (Board) Board->RemoveShipFromGrid(Ship);

    OnShipDestroyed.Broadcast(Ship);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("DestroyShip: %s envoyé à la décharge"),
            *Ship->GetName());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

void UCombatResolver::ResolveCollision(AAShip* ShipA, AAShip* ShipB, FPushResult& OutResult, int32 RemainingDepth)
{
    if (!ShipA || !ShipB) return;

    constexpr int32 CollisionDamage = 2;

    FCollision Collision;
    Collision.ShipA = ShipA;
    Collision.ShipB = ShipB;
    Collision.DamageEach = CollisionDamage;
    OutResult.Collisions.Add(Collision);

    OnCollision.Broadcast(ShipA, ShipB);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ResolveCollision: %s ↔ %s → %d dégâts chacun"),
            *ShipA->GetName(), *ShipB->GetName(), CollisionDamage);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    ApplyDamageToShip(ShipA, CollisionDamage);
    ApplyDamageToShip(ShipB, CollisionDamage);

    if (RemainingDepth > 0 && IsValid(ShipB))
    {
        const FIntPoint Delta = ShipB->GetGridPosition() - ShipA->GetGridPosition();
        EDirections ChainDir = IntPointToDirection(Delta);

        ApplyPush(ShipB, ChainDir, RemainingDepth);
    }
}

FIntPoint UCombatResolver::GetCellBehind(AAShip* Ship, EDirections PushDirection) const
{
	const FIntPoint PushVec = DirectionToIntPoint(PushDirection);
	const FIntPoint Behind = Ship->GetGridPosition() - PushVec;
	return Behind;
}

FIntPoint UCombatResolver::DirectionToIntPoint(EDirections dir) const
{
    return AllDirections[(uint8)dir];
}

EDirections UCombatResolver::IntPointToDirection(FIntPoint Delta)
{
    return EDirections();
}
