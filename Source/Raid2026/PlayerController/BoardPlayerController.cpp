#include "BoardPlayerController.h"
#include "../GameMode/ABoardGameMode.h"
#include <Net/UnrealNetwork.h>
#include "../PlayerState/BoardPlayerState.h"


void ABoardPlayerController::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABoardPlayerController, PlayerID);
    DOREPLIFETIME(ABoardPlayerController, BoardVisualiser);
}

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

void ABoardPlayerController::ClientOnCardDiscarded_Implementation(UUCardData* Card)
{
    OnCardDiscardedBP(Card);
}

void ABoardPlayerController::ClientGetAllDiscardCards_Implementation(const TArray<UUCardData*>& CardDiscards)
{
    GetAllDiscardCardsBP(CardDiscards);
}

void ABoardPlayerController::ClearPendingActivation()
{
    PendingActivationShip = nullptr;
    PendingActivationEffectIndex = -1;
    PendingActivationTargetKind = EEffectTargetKind::None;
}

void ABoardPlayerController::ClientOnEnemyEssenceReceived_Implementation(int32 essence)
{
    OnEnemyEssenceReceived(essence);
}

void ABoardPlayerController::ClientMothershipHealthChanged_Implementation(int32 ownMothership, int32 enemyMothership)
{
    OnMothershipHealthChangedBP(ownMothership, enemyMothership);
}

void ABoardPlayerController::SetupPlayer(int32 ID)
{
    PlayerID = ID;

    ClientInitializeInput();
}

void ABoardPlayerController::OnRep_BoardVisualiser()
{

}

void ABoardPlayerController::OnRep_PlayerID()
{

}

void ABoardPlayerController::ClickOnShip(AAShip* Ship)
{
    if (!bIsMyTurn) return;
    if (!Ship) return;

    if(PendingIntent == EActionIntent::ACTIVATE)
    {
        if (PendingActivationTargetKind == EEffectTargetKind::Ship)
            ServerConfirmEffectTarget(Ship);
        return;
    }
    if (Ship->ownerPlayer == PlayerID)
    {
        if (PendingIntent == EActionIntent::PLAYCARD && IsValid(PendingCardData))
        {
            ServerUpgrade(Ship, PendingCardData);
        }
        else
        {
            HandleShipSelected(Ship);
        }
    }
    else if (PendingIntent == EActionIntent::PLAYCARD && IsValid(PendingCardData))
    {
        FEffectContext context = FEffectContext();
        context.TargetShip = Ship;
        context.OwnerPlayerID = PlayerID;
        ServerPlaySabotage(PendingCardData, context);
    }
    else if (SelectedShip)
    {
        PendingIntent = EActionIntent::FIRE;
        ServerFireAt(SelectedShip, Ship->GetGridPosition());
    }
}

void ABoardPlayerController::ClickOnCell(AABoardCell* Cell)
{
    if (!bIsMyTurn || !IsValid(Cell) || PendingIntent == EActionIntent::NONE) return;

    switch (PendingIntent)
    {
        case EActionIntent::MOVE:

            if (IsValid(SelectedShip))
            {
                ServerMoveShip(SelectedShip, Cell->cellData.Pos);
            }
            break;

        case EActionIntent::PLAYCARD:

            if (PendingShipClass && PendingCardData)
            {
                ServerSpawnShip(PendingShipClass,
                    Cell->cellData.Pos,
                    PendingCardData);
            }
            break;

        case EActionIntent::ACTIVATE:

            if (PendingActivationTargetKind == EEffectTargetKind::Cell)
            {
                ServerConfirmEffectTargetCell(Cell->cellData.Pos);
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

    if (PendingIntent == EActionIntent::PLAYCARD && IsValid(PendingCardData))
    {
        if (PendingCardData.Get()->type == ECardType::EXPERT)
        {
            ServerPlaceExpert(PendingCardData, Mothership);
        }

        else if (PendingCardData.Get()->type == ECardType::SABOTAGE)
        {
            FEffectContext context = FEffectContext();
            context.TargetMothership = Mothership;
            context.OwnerPlayerID = PlayerID;
            ServerPlaySabotage(PendingCardData, context);
        }
        
        return;
    }

    if (IsValid(SelectedShip))
    {
        PendingIntent = EActionIntent::FIRE;
        ServerFireAtMothership(SelectedShip, Mothership);
    }
}

void ABoardPlayerController::OnResolveSabotageTarget(UUCardData* SelectedCard, int32 CardIndex)
{
    if (!IsValid(SelectedCard) || PendingIntent != EActionIntent::ACTIVATE)
    {
        return;
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Card selected : "));
        text += SelectedCard->cardName;
        text += " | index : ";
        text += FString::FromInt(CardIndex);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    ServerResolveSabotageTarget(CardIndex);
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
    if (IsValid(SelectedShip))
    {
        OnShipUnselectedBP(SelectedShip);
    }
    
    SelectedShip = Ship;
    PendingIntent = EActionIntent::MOVE;
    bWaitingForCellTarget = true;
    
    ServerRequestActivatableEffects(Ship);
    ServerRequestReachableCells(Ship);
}

void ABoardPlayerController::ClientPromptSabotageTarget_Implementation(const TArray<UUCardData*>& Candidates)
{
    OnSabotageTargetPromptBP(Candidates);
}

bool ABoardPlayerController::ServerConfirmSabotageTarget_Validate(int32 ChosenIndex)
{
    return ChosenIndex >= 0;
}

bool ABoardPlayerController::ServerConfirmEffectTarget_Validate(AAShip* TargetShip)
{
    return IsValid(TargetShip);
}

bool ABoardPlayerController::ServerConfirmEffectTargetCell_Validate(FIntPoint Cell)
{
    return true;
}

bool ABoardPlayerController::ServerResolveSabotageTarget_Validate(int32 CardIndex)
{
    return CardIndex > -1;
}

void ABoardPlayerController::ServerActivateActiveEffect_Implementation(AAShip* ShipSelected)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    if (!ShipSelected->CanAct()) return;
    
    GM->ActivateActiveEffect(ShipSelected);
}

bool ABoardPlayerController::ServerActivateActiveEffect_Validate(AAShip* ShipSelected)
{
    return IsValid(ShipSelected);
}

void ABoardPlayerController::ServerReviveCard_Implementation(int32 cardIndexInDump)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleReviveCard(this,cardIndexInDump);
}

bool ABoardPlayerController::ServerReviveCard_Validate(int32 cardIndexInDump)
{
    return cardIndexInDump >= 0;
}

void ABoardPlayerController::ServerAskForEnemyEssence_Implementation()
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    ClientOnEnemyEssenceReceived(GM->HandleGetEnemyEssence(PlayerID));
}

void ABoardPlayerController::ServerResolveSabotageTarget_Implementation(int32 CardIndex)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->ResolveSabotageTarget(this, CardIndex);
}

void ABoardPlayerController::ServerConfirmEffectTargetCell_Implementation(FIntPoint Cell)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->ResolveActivationTargetCell(this, Cell);
}

void ABoardPlayerController::ClientPromptEffectTarget_Implementation(EEffectTargetKind Kind)
{
    PendingIntent = EActionIntent::ACTIVATE;
    PendingActivationTargetKind = Kind;
    OnEffectTargetPromptBP(Kind);
}

void ABoardPlayerController::ServerConfirmEffectTarget_Implementation(AAShip* TargetShip)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->ResolveActivationTarget(this, TargetShip);
}

void ABoardPlayerController::ServerRequestActivatableEffects_Implementation(AAShip* Ship)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    if (!GM->CanShipPlay(Ship)) return;
    
    TArray<UEffect*> Effects = GM->effectManager->GetActivatableEffects(Ship, PlayerID);
    bool bHaveActiveEffect = false;
    if (Effects.Num() >= 0)
    {
        for (int i = 0; i < Effects.Num(); i++)
        {
            if (Effects[i]->Trigger == EEffectTrigger::Activated)
            {
                bHaveActiveEffect = true;
            }
        }
    }
    ClientReceiveActivatableEffects(GM->BuildActivatableInfos(this, Ship), Ship, bHaveActiveEffect);
}

void ABoardPlayerController::ClientReceiveActivatableEffects_Implementation(const TArray<FActivatableEffectInfo>& Infos, AAShip* Ship, bool bHaveActiveEffect)
{
    OnShipSelectedBP(Ship, bHaveActiveEffect);
    OnActivatableEffectsReceivedBP(Infos);
}

void ABoardPlayerController::ServerConfirmSabotageTarget_Implementation(int32 ChosenIndex)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;
    GM->ResolveSabotageTarget(this, ChosenIndex);
}

void ABoardPlayerController::HandleCellTargeted(AABoardCell* Cell)
{

}

void ABoardPlayerController::ClearSelection()
{
    ClientClearSelection();
}

void ABoardPlayerController::ClientClearSelection_Implementation()
{
    if (IsValid(SelectedShip))
    {
        OnShipUnselectedBP(SelectedShip);
        if (!SelectedShip->CanAct())
        {
            SelectedShip->StopShip();
        }
        SelectedShip = nullptr;
    }

    PendingIntent = EActionIntent::NONE;
    PendingShipClass = nullptr;
    PendingCardData = nullptr;
    bWaitingForCellTarget = false;

    ClearPendingActivation();
    OnClearSelectionBP();

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
    return IsValid(Ship);
}

void ABoardPlayerController::ServerMoveShip_Implementation(
    AAShip* Ship, FIntPoint TargetCell)
{
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("can move : "));
        text += Ship->CanMove() ? "true" : "false";

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    if (!Ship->CanMove()) return;

    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleMoveShip(this, Ship, TargetCell);
}

bool ABoardPlayerController::ServerFireAt_Validate(
    AAShip* Shooter, FIntPoint TargetCell)
{
    return IsValid(Shooter);
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
    return IsValid(Ship) && IsValid(Mothership);
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
    return IsValid(Card);
}

void ABoardPlayerController::ServerPlaceExpert_Implementation(UUCardData* Card, AAMotherShip* Mothership)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandlePlaceExpert(this, Card);
}

bool ABoardPlayerController::ServerPlaceExpert_Validate(UUCardData* Card, AAMotherShip* Mothership)
{
    return IsValid(Card) && IsValid(Mothership);
}

bool ABoardPlayerController::ServerActivateEffect_Validate(AAShip* Ship, int32 EffectIndex)
{
    return IsValid(Ship) && EffectIndex >= 0;
}

void ABoardPlayerController::Server_GetAllDiscardsCards_Implementation(bool bIsPlayerDiscardDeck)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleGetAllDiscardCards(this,bIsPlayerDiscardDeck);
}

void ABoardPlayerController::ServerActivateEffect_Implementation(AAShip* Ship, int32 EffectIndex)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandleActivateEffect(this, Ship, EffectIndex);
}

void ABoardPlayerController::Server_RevealShip_Implementation(AAShip* Ship)
{
    Ship->Reveal();
    ClearSelection();
}

bool ABoardPlayerController::ServerUpgrade_Validate(AAShip* Ship, UUCardData* Card)
{
    return IsValid(Ship) && IsValid(Card);
}

bool ABoardPlayerController::ServerPlaySabotage_Validate(UUCardData* Card, const FEffectContext& context)
{
    return IsValid(Card);
}

void ABoardPlayerController::ServerPlaySabotage_Implementation(UUCardData* Card, const FEffectContext& context)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandlePlaySabotage(this, Card, context);
}

void ABoardPlayerController::ServerUpgrade_Implementation(AAShip* Ship, UUCardData* Card)
{
    AABoardGameMode* GM = GetWorld()->GetAuthGameMode<AABoardGameMode>();
    if (!GM) return;

    GM->HandlePlaceUpgrade(this, Card, Ship);
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
    return IsValid(ShipClass) && IsValid(CardData);
}

void ABoardPlayerController::ClientOnPlayCard_Implementation()
{
    OnCardPlayedBP();
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

    TArray<FIntPoint> Reachable = GM->PathFinder->GetAllCellAroundShip(Ship);

    ClientOnReachableCells(Reachable);
}

void ABoardPlayerController::ClientOnTurnStarted_Implementation(
    int32 ActivePlayerID)
{
    bIsMyTurn = (ActivePlayerID == PlayerID);

    OnTurnStartedBP(ActivePlayerID, bIsMyTurn);
    ServerAskForEnemyEssence();

    if (!bIsMyTurn)
        ClearSelection();
}

void ABoardPlayerController::ClientOnTurnEnded_Implementation()
{
    bIsMyTurn = false;
    ClearSelection();
    ClearPendingActivation();
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
        BoardVisualiser->ClearHighlights();
    }
}

void ABoardPlayerController::ClientOnShipDestroyed_Implementation(
    AAShip* Ship)
{
    if (!Ship || Ship->IsPendingKillPending())
    {
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
    {
        BoardVisualiser->ClearHighlights();
        BoardVisualiser->HighlightCells(Cells);
    }
}

void ABoardPlayerController::ClientOnVictory_Implementation(int32 WinnerID)
{
    OnVictoryBP(WinnerID);
}