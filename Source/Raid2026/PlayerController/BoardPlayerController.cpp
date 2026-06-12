#include "BoardPlayerController.h"
#include "../GameMode/ABoardGameMode.h"
#include <Net/UnrealNetwork.h>
#include "../PlayerState/BoardPlayerState.h"

void ABoardPlayerController::ClientInitializeInput_Implementation()
{
    if (!InputComponent)
    {
        InitInputSystem();
    }

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(BoardMappingContext, 0);
    }

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    InitializeInputBP();
}

void ABoardPlayerController::ClientOnCardDrawn_Implementation(UUCardData* Card)
{
    OnCardDrawnBP(Card);
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

    if (Ship->ownerPlayer == PlayerID && Ship->CanBePlayed())
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("Selecting ship"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }
        HandleShipSelected(Ship);
    }
    else if (SelectedShip)
    {
        PendingIntent = EActionIntent::FIRE;
        ServerFireAt(SelectedShip, Ship->GetGridPosition());
        //ClearSelection();
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
            //ClearSelection();
        }
        break;

    case EActionIntent::SPAWNSHIP:

        if (PendingShipClass && PendingCardData)
        {
            ServerSpawnShip(PendingShipClass,
                Cell->cellData.Pos,
                PendingCardData);
            OnCardPlayedBP();
            //ClearSelection();
        }
        break;

    default:
        ClearSelection();
        break;
    }
}

void ABoardPlayerController::ClickOnMotherShip(AAMotherShip* Mothership)
{
    if (!IsValid(Mothership)) return;

    if (IsValid(SelectedShip))
    {
        PendingIntent = EActionIntent::FIRE;
        ServerFireAtMothership(SelectedShip, Mothership);
    }

    //ClearSelection();
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
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("handle ship selected"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
    SelectedShip = Ship;
    PendingIntent = EActionIntent::MOVE;
    bWaitingForCellTarget = true;
    OnShipSelectedBP();

    ServerRequestReachableCells(Ship);
}

void ABoardPlayerController::HandleCellTargeted(AABoardCell* Cell)
{
}

void ABoardPlayerController::ClearSelection()
{
    if (IsValid(SelectedShip))
    {
        SelectedShip->bJustPlayed = true;
        SelectedShip = nullptr;
    }
    
    PendingIntent = EActionIntent::NONE;
    PendingShipClass = nullptr;
    PendingCardData = nullptr;
    bWaitingForCellTarget = false;

    if (BoardVisualiser)
        BoardVisualiser->ClearHighlights();
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
    return Ship != nullptr && !Ship->bHasMoved;
}

void ABoardPlayerController::ServerMoveShip_Implementation(
    AAShip* Ship, FIntPoint TargetCell)
{
    if (Ship->bHasMoved) return;

    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleMoveShip(this, Ship, TargetCell);
}

bool ABoardPlayerController::ServerFireAt_Validate(
    AAShip* Shooter, FIntPoint TargetCell)
{
    return Shooter != nullptr && Shooter->CanAct();
}

void ABoardPlayerController::ServerFireAtMothership_Implementation(AAShip* Ship, AAMotherShip* Mothership)
{
    if (!Ship->CanAct()) return;

    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleFireAtMothership(this, Ship, Mothership);
}

bool ABoardPlayerController::ServerFireAtMothership_Validate(AAShip* Ship, AAMotherShip* Mothership)
{
    return Ship != nullptr && Mothership != nullptr && Ship->CanAct();
}

void ABoardPlayerController::ServerFireAt_Implementation(
    AAShip* Shooter, FIntPoint TargetCell)
{
    if (!Shooter->CanAct()) return;

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
    if (BoardVisualiser)
        BoardVisualiser->HighlightCells(Cells);
}

void ABoardPlayerController::ClientOnVictory_Implementation(int32 WinnerID)
{
    OnVictoryBP(WinnerID);
}