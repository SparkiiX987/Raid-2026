#include "CombatResolver.h"

static const TArray<FIntPoint> AllDirections =
{
	FIntPoint(0,  1),  // Nord
	FIntPoint(0, -1),  // Sud
	FIntPoint(-1,  0),  // Ouest
	FIntPoint(1,  0)  // Est
};

FFireResult UCombatResolver::ResolveFire(AActor* Shooter, FIntPoint TargetCell)
{
    FFireResult Result;

    /*if (!Shooter)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("ResolveFire: Shooter est null"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return Result;
    }

    if (!HasLineOfSight(Shooter, TargetCell))
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("ResolveFire: Pas de LdV vers (%d,%d)"),
                TargetCell.X, TargetCell.Y);

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return Result;
    }

    UBoardManager* Board = GetBoard();
    if (!Board) return Result;

    if (Shooter->IsFaceDown())
    {
        Shooter->Reveal();
    }

    AShipActor* TargetShip = Board->GetShipAt(TargetCell);

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

    AMothershipActor* TargetMothership = Board->GetMothershipAt(TargetCell);
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
    }*/
    return Result;
}

void UCombatResolver::ApplyDamageToShip(AActor* Target, int32 Damage)
{
    if (!Target || Damage <= 0) return;

    /*Target->TakeDamage(Damage);
    OnShipDamaged.Broadcast(Target, Damage);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ApplyDamage: %s reçoit %d dégâts → résistance = %d"),
            *Target->GetName(), Damage, Target->GetCurrentResistance());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    if (Target->GetCurrentResistance() <= 0)
    {
        DestroyShip(Target);
    }*/
}

void UCombatResolver::ApplyDamageToMothership(AActor* Target, int32 Damage)
{
    if (!Target ||Damage <= 0) return;

    /*Target->TakeDamage(Damage);
    OnMothershipDamaged.Broadcast(Target, Damage);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ApplyDamage: Vaisseau-mère reçoit %d dégâts → PV = %d"),
            Damage, Target->GetCurrentHP());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }*/
}

FPushResult UCombatResolver::ApplyPush(AActor* Ship, EDirections Direction, int32 MaxChainDepth)
{
    FPushResult Result;

    /*if (!Ship || MaxChainDepth <= 0)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("ApplyPush: Ship null ou profondeur max atteinte"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        return Result;
    }

    UBoardManager* Board = GetBoard();
    if (!Board) return Result;

    const FIntPoint Behind = GetCellBehind(Ship, Direction);

    if (Board->IsValidCell(Behind) && !Board->IsCellOccupied(Behind))
    {
        Board->MoveShipTo(Ship, Behind);
        Result.bMoved = true;
        Result.FinalCell = Behind;
        return Result;
    }

    AShipActor* CollidingShip = Board->GetShipAt(Behind);

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
    */
    return Result;
}

bool UCombatResolver::HasLineOfSight(AActor* Shooter, FIntPoint TargetCell) const
{
    if (!Shooter) return false;

    if (!Board)   return false;

    /*const FIntPoint From = Shooter->GetGridPosition();

    const bool bSameCol = (From.X == TargetCell.X);
    const bool bSameRow = (From.Y == TargetCell.Y);

    if (!bSameCol && !bSameRow) return false;
    if (From == TargetCell)     return false;

    FIntPoint Step = FIntPoint::ZeroValue;
    if (bSameCol) Step.Y = (TargetCell.Y > From.Y) ? 1 : -1;
    else          Step.X = (TargetCell.X > From.X) ? 1 : -1;

    FIntPoint Current = From + Step;
    while (Current != TargetCell)
    {
        if (Board->IsCellOccupied(Current))
            return false;

        Current += Step;
    }
    return Board->IsValidCell(TargetCell);
    */
    return true;
}

TArray<FIntPoint> UCombatResolver::GetValidFireTargets(AActor* Shooter) const
{
    TArray<FIntPoint> ValidTargets;
    if (!Shooter) return ValidTargets;

    if (!Board)   return ValidTargets;

    /*const FIntPoint Origin = Shooter->GetGridPosition();
    const int32     Radar = Shooter->GetRadarRange();

    const UBoardManager* board = GetBoard();

    const TArray<FIntPoint> Orthogonals = 
    {
        { 0,  1}, { 0, -1}, {-1,  0}, { 1,  0}
    };

    for (const FIntPoint& Dir : Orthogonals)
    {
        FIntPoint Current = Origin + Dir;
        int32     Steps = 0;

        while (Steps < Radar && board->IsValidCell(Current))
        {
            AShipActor* OccupantShip = board->GetShipAt(Current);

            if (OccupantShip)
            {
                if (OccupantShip->GetOwnerID() != Shooter->GetOwnerID()
                    && !OccupantShip->IsFaceDown())
                {
                    ValidTargets.Add(Current);
                }
                break;
            }

            if (board->IsMothershipCell(Current, Shooter->GetOwnerID()))
            {
                ValidTargets.Add(Current);
                break;
            }

            Current += Dir;
            ++Steps;
        }
    }*/

    return ValidTargets;
}

void UCombatResolver::DestroyShip(AActor* Ship)
{
    if (!Ship) return;

   /* UBoardManager* Board = GetBoard();
    if (Board) Board->RemoveShipFromGrid(Ship);*/

    OnShipDestroyed.Broadcast(Ship);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("DestroyShip: %s envoyé à la décharge"),
            *Ship->GetName());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

void UCombatResolver::ResolveCollision(AActor* ShipA, AActor* ShipB, FPushResult& OutResult, int32 RemainingDepth)
{
    if (!ShipA || !ShipB) return;

    constexpr int32 CollisionDamage = 2;

    FCollision Collision;
    Collision.ShipA = ShipA;
    Collision.ShipB = ShipB;
    Collision.DamageEach = CollisionDamage;
    OutResult.Collisions.Add(Collision);

    OnCollision.Broadcast(ShipA, ShipB);

    UE_LOG(LogTemp, Log,
        TEXT("ResolveCollision: %s ↔ %s → %d dégâts chacun"),
        *ShipA->GetName(), *ShipB->GetName(), CollisionDamage);

    ApplyDamageToShip(ShipA, CollisionDamage);
    ApplyDamageToShip(ShipB, CollisionDamage);

    /*if (RemainingDepth > 0 && IsValid(ShipB))
    {
        const FIntPoint Delta = ShipB->GetGridPosition() - ShipA->GetGridPosition();
        EDirections ChainDir = IntPointToDirection(Delta);

        ApplyPush(ShipB, ChainDir, RemainingDepth);
    }*/
}

FIntPoint UCombatResolver::GetCellBehind(AActor* Ship, EDirections PushDirection) const
{
	//const FIntPoint PushVec = DirectionToIntPoint(PushDirection);
	const FIntPoint Behind =/* Ship->GetGridPosition() - PushVec*/FIntPoint();
	return Behind;
}