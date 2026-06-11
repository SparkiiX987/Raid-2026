#include "BoardPlayerController.h"
#include "../GameMode/ABoardGameMode.h"
#include <Net/UnrealNetwork.h>
#include "../PlayerState/BoardPlayerState.h"

void ABoardPlayerController::ClientInitializeInput_Implementation()
{
    InitializeInputBP();
}

void ABoardPlayerController::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABoardPlayerController, PlayerID);
}

void ABoardPlayerController::SetupPlayer(int32 ID)
{
    PlayerID = ID;
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("PlayerController: received PlayerID %d"), PlayerID);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
    ClientInitializeInput();
}

void ABoardPlayerController::OnRep_PlayerID()
{
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("PlayerController: received PlayerID %d"), PlayerID);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

void ABoardPlayerController::ClickOnShip(AAShip* Ship)
{
    if (!bIsMyTurn) return;
    if (!Ship) return;

    if (Ship->ownerPlayer == PlayerID)
    {
        HandleShipSelected(Ship);
    }
    else if (SelectedShip)
    {
        PendingIntent = EActionIntent::FIRE;
        ServerFireAt(SelectedShip, Ship->GetGridPosition());
        ClearSelection();
    }
}

void ABoardPlayerController::ClickOnCell(AABoardCell* Cell)
{
    if (!bIsMyTurn) return;
    if (!Cell) return;

    switch (PendingIntent)
    {
    case EActionIntent::MOVE:
        if (SelectedShip)
        {
            ServerMoveShip(SelectedShip, Cell->cellData.Pos);
            ClearSelection();
        }
        break;

    case EActionIntent::SPAWNSHIP:

        if (PendingShipClass && PendingCardData)
        {
            ServerSpawnShip(PendingShipClass,
                Cell->cellData.Pos,
                PendingCardData);
            ClearSelection();
        }
        break;

    default:
        break;
    }
}

void ABoardPlayerController::ClickOnMotherShip(AAMotherShip* Mothership)
{
    if (!IsValid(Mothership)) return;

    PendingIntent = EActionIntent::FIRE;
    ServerFireAtMothership(SelectedShip, Mothership);
    ClearSelection();
}

void ABoardPlayerController::RequestEndTurn()
{
    if (!bIsMyTurn) return;
    ServerEndTurn();
}

void ABoardPlayerController::RequestPlayCard(
    UUCardData* Card, FIntPoint TargetCell)
{
    if (!bIsMyTurn || !Card) return;
    ServerPlayCard(Card, TargetCell);
}

void ABoardPlayerController::HandleShipSelected(AAShip* Ship)
{
    SelectedShip = Ship;
    PendingIntent = EActionIntent::MOVE;
    bWaitingForCellTarget = true;

    ServerRequestReachableCells(Ship);
}

void ABoardPlayerController::HandleCellTargeted(AABoardCell* Cell)
{
}

void ABoardPlayerController::ClearSelection()
{
    SelectedShip = nullptr;
    PendingIntent = EActionIntent::NONE;
    PendingShipClass = nullptr;
    PendingCardData = nullptr;
    bWaitingForCellTarget = false;

    /*if (BoardVisualiser)
        BoardVisualiser->ClearHighlights();*/
}

void ABoardPlayerController::ServerSetupFinish_Implementation()
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->PlayerSetupFinished(this, PlayerID);
}

bool ABoardPlayerController::ServerSetupFinish_Validate()
{
    return PlayerID != -1;
}

bool ABoardPlayerController::ServerMoveShip_Validate(
    AAShip* Ship, FIntPoint TargetCell)
{
    return Ship != nullptr;
}

void ABoardPlayerController::ServerMoveShip_Implementation(
    AAShip* Ship, FIntPoint TargetCell)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleMoveShip(this, Ship, TargetCell);
}

bool ABoardPlayerController::ServerFireAt_Validate(
    AAShip* Shooter, FIntPoint TargetCell)
{
    return Shooter != nullptr;
}

void ABoardPlayerController::ServerFireAtMothership_Implementation(AAShip* Ship, AAMotherShip* Mothership)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleFireAtMothership(this, Ship, Mothership);
}

bool ABoardPlayerController::ServerFireAtMothership_Validate(AAShip* Ship, AAMotherShip* Mothership)
{
    return Ship != nullptr && Mothership != nullptr;
}

void ABoardPlayerController::ServerFireAt_Implementation(
    AAShip* Shooter, FIntPoint TargetCell)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleFireAt(this, Shooter, TargetCell);
}

bool ABoardPlayerController::ServerPlayCard_Validate(
    UUCardData* Card, FIntPoint TargetCell)
{
    return Card != nullptr;
}

void ABoardPlayerController::ServerPlayCard_Implementation(
    UUCardData* Card, FIntPoint TargetCell)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandlePlayCard(this, Card, TargetCell);
    OnCardPlayedBP();
}

void ABoardPlayerController::ServerEndTurn_Implementation()
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleEndTurn(this);
}

bool ABoardPlayerController::ServerSpawnShip_Validate(
    TSubclassOf<AAShip> ShipClass,
    FIntPoint TargetCell,
    UUCardData* CardData)
{
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("ServerSpawnShip_Validate: ShipClass=%s CardData=%s"),
            ShipClass ? *ShipClass->GetName() : TEXT("null"),
            CardData ? *CardData->GetName() : TEXT("null"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    return IsValid(ShipClass) && IsValid(CardData);
}

void ABoardPlayerController::ServerSpawnShip_Implementation(
    TSubclassOf<AAShip> ShipClass,
    FIntPoint TargetCell,
    UUCardData* CardData)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleSpawnShip(this, ShipClass, TargetCell, CardData);
}

void ABoardPlayerController::ServerRequestReachableCells_Implementation(
    AAShip* Ship)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    TArray<FIntPoint> Reachable = GM->GetReachableCellsForShip(this, Ship);
    ClientOnReachableCells(Reachable);
}

void ABoardPlayerController::ClientOnTurnStarted_Implementation(
    int32 ActivePlayerID)
{
    bIsMyTurn = (ActivePlayerID == PlayerID);

    OnTurnStartedBP(ActivePlayerID, bIsMyTurn);

    if (!bIsMyTurn)
        ClearSelection();
}

void ABoardPlayerController::ClientOnTurnEnded_Implementation()
{
    bIsMyTurn = false;
    ClearSelection();
    OnTurnEndedBP();
}

void ABoardPlayerController::ClientUpdateHand_Implementation(
    const TArray<UUCardData*>& NewHand)
{
    ABoardPlayerState* PS = GetPlayerState<ABoardPlayerState>();
    if (PS)
        PS->SetHand(NewHand);

    OnHandUpdatedBP(NewHand);
}

void ABoardPlayerController::ClientOnActionRejected_Implementation(
    const FString& Reason)
{
    ClearSelection();
    OnActionRejectedBP(Reason);
}

void ABoardPlayerController::ClientOnShipMoved_Implementation(
    AAShip* Ship, FIntPoint NewCell)
{
    if (BoardVisualiser && Ship)
    {
        FVector WorldPos = BoardVisualiser->GridToWorld(NewCell);
        Ship->SetActorLocation(WorldPos);
    }
}

void ABoardPlayerController::ClientOnShipDestroyed_Implementation(
    AAShip* Ship)
{
    if (!Ship || Ship->IsPendingKillPending())
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("Ship déjà destroyed ou en cours"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }

        return;
    }

    if (SelectedShip == Ship)
        ClearSelection();

    OnShipDestroyedBP(Ship);
}

void ABoardPlayerController::ClientOnEssenceChanged_Implementation(
    int32 NewEssence, int32 MaxEssence)
{
    OnEssenceChangedBP(NewEssence, MaxEssence);
}

void ABoardPlayerController::ClientOnReachableCells_Implementation(
    const TArray<FIntPoint>& Cells)
{
    /*if (BoardVisualiser)
        BoardVisualiser->HighlightCells(Cells);*/
}

void ABoardPlayerController::ClientOnVictory_Implementation(int32 WinnerID)
{
    OnVictoryBP(WinnerID);
}