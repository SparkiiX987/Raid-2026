#include "ABoardGameMode.h"
#include "../CoreLayer/Cells/ReplicatedCellState.h"
#include "../CoreLayer/Cells/FReachableCell.h"
#include "../GameState/BoardGameState.h"
#include "../PlayerState/BoardPlayerState.h"
#include <Raid2026/Effects/Effect.h>
#include <Raid2026/Effects/ResearchAndDeveloppement.h>

void AABoardGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void AABoardGameMode::HandleSeamlessTravelPlayer(AController*& Controller)
{
    Super::HandleSeamlessTravelPlayer(Controller);

    ABoardPlayerController* PC =
        Cast<ABoardPlayerController>(Controller);
    if (!PC) return;

    int32 AssignedID = NextPlayerID++;
    PC->SetupPlayer(AssignedID);
    
    ABoardPlayerState* PState = PC->GetPlayerState<ABoardPlayerState>();
    
    if (!PState)
    {
        return;
    }

    PState->SetPlayerId(AssignedID);
    PState->InitializeDeckBP();

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("GameMode: Player %d connected (%d/%d)"),
            AssignedID, connectedPlayers.Num(), playerCount);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

void AABoardGameMode::Logout(AController* Exiting)
{
    ABoardPlayerController* PC =
        Cast<ABoardPlayerController>(Exiting);
    if (PC)
        connectedPlayers.Remove(PC->PlayerID);

    Super::Logout(Exiting);
}

void AABoardGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
}

void AABoardGameMode::StartGameWhenReady()
{
    if (connectedPlayers.Num() < playerCount) return;

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("GameMode: Player connected : %d"),
            connectedPlayers.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, text);
    }

    StartGame();
}

void AABoardGameMode::StartGame()
{
    if (connectedPlayers.Num() < playerCount)
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
                TEXT("StartGame appel� trop t�t, abandon"));
        return;
    }

    SpawnManagers();
    SpawnBoardActors();
    SpawnVisualiser();

    OnSpawnFinishedBP();

    deckManager->OnCardDrawn.AddDynamic(this, &AABoardGameMode::HandleCardDrawn);
    turnManager->OnTurnStarted.AddDynamic(this, &AABoardGameMode::HandleTurnStarted);

    turnManager->OnEssenceSpent.AddDynamic(this, &AABoardGameMode::HandleEssenceSpent);
    turnManager->OnBonusEssenceGained.AddDynamic(this, &AABoardGameMode::HandleBonusEssenceGained);

    turnManager->InitializeGame(0, playerCount);
    effectManager->Initialize(boardManager, turnManager, deckManager, combatResolver);

    for (auto& [PlayerId, PC] : connectedPlayers)
    {
        BroadcastEssenceChanged(PlayerId);
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("GameMode: Player connected : %d"),
            connectedPlayers.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    for (auto& [ID, PC] : connectedPlayers)
    {
        ABoardPlayerState* PS =
            PC->GetPlayerState<ABoardPlayerState>();
        
        if (!PS)
        {
            continue;
        }

        if (TArray<UUCardData*>* DeckPtr = PS->allDecks.Find(ID))
        {
            deckManager->InitializeDeck(ID, *DeckPtr);
        }
    }

    turnManager->StartTurn(0);
    OnInitialisationFinishedBP();
}

void AABoardGameMode::PlayerSetupFinished(ABoardPlayerController* PC, int32 playerId)
{
    connectedPlayers.Add(playerId, PC);

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("GameMode: Player setuped : %d"),
            playerId);

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    StartGameWhenReady();
}

void AABoardGameMode::SpawnManagers()
{
    UWorld* World = GetWorld();

    boardManager = NewObject<UUBoardManager>(this, boardManagerClass);
    turnManager = NewObject<UUTurnManager>(this, turnManagerClass);
    deckManager = NewObject<UDeckManager>(this, deckManagerClass);
    combatResolver = NewObject<UCombatResolver>(this, combatResolverClass);
    effectManager = NewObject<UEffectManager>(this, effectManagerClass);
    upgradesManager = NewObject<UUpgradesManager>(this, upgradeManagerClass);
    PathFinder = NewObject<UPathFinder>(this, pathFinderClass);

    turnManager->boardManager = boardManager;
    turnManager->deckManager = deckManager;
    turnManager->effectManager = effectManager;

    combatResolver->Board = boardManager;

    PathFinder->BoardManager = boardManager;
    PathFinder->TurnManager = turnManager;

    boardManager->OnVictoryConditionMet.AddDynamic(
        this, &AABoardGameMode::OnVictoryConditionMet);

    boardManager->OnRefineryControlChanged.AddDynamic(
        this, &AABoardGameMode::OnRefineryControlChanged);
}

void AABoardGameMode::SpawnVisualiser()
{
    if (!boardVisualiserClass)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("SpawnVisualiser: boardVisualiserClass non assign�e !"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
        return;
    }

    boardVisualiser = GetWorld()->SpawnActor<AABoardVisualiser>(
        boardVisualiserClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator);

    if (!boardVisualiser)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("SpawnVisualiser: �chec du spawn !"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
        return;
    }

    for (TTuple<int, TObjectPtr<ABoardPlayerController>>& Pair : connectedPlayers)
    {
        if (Pair.Value)
        {
            Pair.Value->BoardVisualiser = boardVisualiser;
        }
    }


    boardVisualiser->BoardManager = boardManager;
    boardVisualiser->TurnManager = turnManager;
    boardVisualiser->SpawnCellActors();
}

void AABoardGameMode::SpawnBoardActors()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AARefinery*> Refineries;
    for (int32 i = 0; i < 2; i++)
    {
        AARefinery* R = World->SpawnActor<AARefinery>(
            refineryClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (R) Refineries.Add(R);
    }

    AAMotherShip* MS0 = World->SpawnActor<AAMotherShip>(
        mothershipClass, FVector::ZeroVector, FRotator::ZeroRotator);
    AAMotherShip* MS1 = World->SpawnActor<AAMotherShip>(
        mothershipClass, FVector::ZeroVector, FRotator::ZeroRotator);

    boardManager->InitializeBoard(Refineries, MS0, MS1);
}

void AABoardGameMode::HandleMoveShip(
    ABoardPlayerController* playerInstigator,
    AAShip* Ship, FIntPoint TargetCell)
{
    if (!ValidateIsPlayerTurn(playerInstigator)) return;
    if (!ValidateShipOwnership(playerInstigator, Ship)) return;
    if (!Ship->CanMove()) { RejectAction(playerInstigator, TEXT("Vaisseau immobilisé")); return; }

    TArray<FIntPoint> PathToTake = PathFinder->InitializeCheck(Ship, boardManager->GetCell(TargetCell));
    if (PathToTake.Num() == 0) return;

    const int32 Distance = PathToTake.Num() - 1;
    const int32 PlayerID = playerInstigator->PlayerID;

    if (Distance > Ship->GetCurrentSpeed())
    {
        RejectAction(playerInstigator, TEXT("Trajet plus long que la vitesse restante"));
        return;
    }

    if (!turnManager->PayEssence(PlayerID, Distance))
    {
        RejectAction(playerInstigator, TEXT("Essence insuffisante"));
        return;
    }

    Ship->OnMove(Distance);
    boardManager->MoveShipTo(Ship, PathToTake);

    if (boardManager->GetCell(TargetCell).Type == ECellType::Refinery
        && effectManager->CanCaptureRefinery(Ship))
    {
        boardManager->GetCell(TargetCell).refinery->Capture(Ship->ownerPlayer);
    }

    UpdateGridState();
    BroadcastEssenceChanged(PlayerID);

    if (!Ship->CanBePlayed())
        playerInstigator->ClearSelection();

    for (auto& [ID, PC] : connectedPlayers)
        PC->ClientOnShipMoved(Ship, TargetCell);

    CheckVictoryConditions();
}

void AABoardGameMode::HandleEssenceSpent(int32 PlayerId, int32 Amount, bool bWasBonus)
{
    BroadcastEssenceChanged(PlayerId);
}

void AABoardGameMode::HandleBonusEssenceGained(int32 PlayerId, int32 Amount)
{
    BroadcastEssenceChanged(PlayerId);
}

void AABoardGameMode::HandlePlaySabotage(ABoardPlayerController* PC, UUCardData* Card, const FEffectContext& context)
{
    if (!ValidateIsPlayerTurn(PC)) return;
    if (!IsValid(Card) || !IsValid(Card->Sabotage)) { RejectAction(PC, TEXT("Carte invalide")); return; }
    if (!deckManager->IsCardInHand(PC->PlayerID, Card)) { RejectAction(PC, TEXT("Carte absente de la main")); return; }

    FEffectContext ctx = context;
    ctx.OwnerPlayerID = PC->PlayerID;

    const FEffectResult Result = effectManager->ActivateEffect(Card->Sabotage, ctx);

    if (Result.bNeedsTarget)
    {
        PC->PendingSabotageCard = Card;
        PC->PendingSabotageContext = ctx;
        PC->ClientPromptSabotageTarget(Result.AffectedCards);
        return;
    }

    if (!Result.bSuccess) { RejectAction(PC, Result.FailReason); return; }

    FinalizeSabotage(PC, Card, Card->Sabotage->EssenceCost);
}

void AABoardGameMode::HandleActivateEffect(ABoardPlayerController* PC, AAShip* Ship, int32 EffectIndex)
{
    if (!ValidateIsPlayerTurn(PC)) return;
    if (!ValidateShipOwnership(PC, Ship)) return;

    FEffectContext ctx;
    ctx.SourceShip = Ship;
    ctx.OwnerPlayerID = PC->PlayerID;
    ctx.TargetShip = PC->SelectedShip;

    TArray<UEffect*> Activatables = effectManager->GetAvailableActivatedEffects(Ship, ctx);
    if (!Activatables.IsValidIndex(EffectIndex)) { RejectAction(PC, TEXT("Effet active indisponible")); return; }

    const FEffectResult Result = effectManager->ActivateEffect(Activatables[EffectIndex], ctx);

    if (!Result.bSuccess && !Result.bNeedsTarget) { RejectAction(PC, Result.FailReason); return; }

    BroadcastEssenceChanged(PC->PlayerID);
    SyncHandToPlayer(PC->PlayerID);
    UpdateGridState();
}

void AABoardGameMode::ResolveSabotageTarget(ABoardPlayerController* PC, int32 ChosenIndex)
{
    UUCardData* Card = PC->PendingSabotageCard;
    if (!IsValid(Card) || !IsValid(Card->Sabotage)) { RejectAction(PC, TEXT("Aucun sabotage en attente")); return; }

    FEffectContext ctx = PC->PendingSabotageContext;
    const int32 cost = Card->Sabotage->EssenceCost;

    if (AAShip* Ship = Cast<AAShip>(ctx.TargetShip.Get()))
    {
        if (!Ship->upgrades.IsValidIndex(ChosenIndex)) { RejectAction(PC, TEXT("Index invalide")); return; }
        ctx.targetedUpgrade = Ship->upgrades[ChosenIndex];

        const FEffectResult Result = effectManager->ActivateEffect(Card->Sabotage, ctx);
        if (!Result.bSuccess) { RejectAction(PC, Result.FailReason); return; }
    }
    else if (AAMotherShip* MS = ctx.TargetMothership.Get())
    {
        if (!MS->RDCards.IsValidIndex(ChosenIndex)) { RejectAction(PC, TEXT("Index invalide")); return; }
        MS->RemoveRDCard(ChosenIndex);
        RebuildMothershipEffects(MS);
    }
    else { RejectAction(PC, TEXT("Cible disparue")); return; }

    PC->PendingSabotageCard = nullptr;
    FinalizeSabotage(PC, Card, cost);
}

void AABoardGameMode::FinalizeSabotage(ABoardPlayerController* PC, UUCardData* Card, int32 cost)
{
    if (!turnManager->PayEssence(PC->PlayerID, cost)) { RejectAction(PC, TEXT("Pas assez d'essence")); return; }

    deckManager->DiscardCard(PC->PlayerID, Card);
    PC->ClientOnPlayCard();
    PC->ClearSelection();
}

void AABoardGameMode::RebuildMothershipEffects(AAMotherShip* MS)
{
    effectManager->UnregisterEffects(MS);

    TArray<UEffect*> Effects;
    for (UUCardData* Card : MS->RDCards)
    {
        if (!Card) continue;
        for (const TObjectPtr<UEffect>& Template : Card->Effects)
            if (Template) Effects.Add(DuplicateObject<UEffect>(Template, MS));
    }
    effectManager->RegisterEffects(MS, Effects);
}
void AABoardGameMode::HandleFireAtMothership(ABoardPlayerController* playerInstigator, AAShip* ship, AAMotherShip* TargetMothership)
{
    FFireResult Result = combatResolver->ResolveFireMothership(ship, TargetMothership);

    if (Result.bMothershipHit && Result.bShipDestroyed)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("Victoire du joueur %d !"), ship->ownerPlayer);

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }

        ship->OnAct();

        if (!ship->CanBePlayed())
        {
            playerInstigator->ClearSelection();
        }

        OnVictoryConditionMet(playerInstigator->PlayerID);
        return;
    }
}

void AABoardGameMode::HandleFireAt(
    ABoardPlayerController* playerInstigator,
    AAShip* Shooter, FIntPoint TargetCell)
{
    if (!ValidateIsPlayerTurn(playerInstigator)) return;
    if (!ValidateShipOwnership(playerInstigator, Shooter)) return;

    int32 PlayerID = playerInstigator->PlayerID;

    if (!turnManager->PayEssence(PlayerID, UUTurnManager::fireCost))
    {
        RejectAction(playerInstigator, "Not enough essence");
        return;
    }

    FFireResult Result = combatResolver->ResolveFire(Shooter, TargetCell);

    if (Result.bMothershipHit && Result.bShipDestroyed)
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("Victoire du joueur %d !"), Shooter->ownerPlayer);

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }

        OnVictoryConditionMet(playerInstigator->PlayerID);
        return;
    }

    Shooter->OnAct();
    BroadcastFireResult(Result);
    BroadcastEssenceChanged(PlayerID);
    UpdateGridState();

    if (!Shooter->CanBePlayed())
    {
        playerInstigator->ClearSelection();
    }

    CheckVictoryConditions();
}

void AABoardGameMode::HandleSpawnShip(
    ABoardPlayerController* playerInstigator,
    TSubclassOf<AAShip> ShipClass,
    FIntPoint TargetCell,
    UUCardData* CardData)
{
    if (!ValidateIsPlayerTurn(playerInstigator)) return;

    int32 PlayerID = playerInstigator->PlayerID;

    if (CardData->type != ECardType::SHIP)
    {
        RejectAction(playerInstigator, "La carte selectionner n'est pas un vaisseau");
        return;
    }

    if (!boardManager->GetFreeSpawnCells(PlayerID).Contains(TargetCell))
    {
        RejectAction(playerInstigator, "Invalid spawn cell");
        return;
    }

    if (!deckManager->IsCardInHand(PlayerID, CardData))
    {
        RejectAction(playerInstigator, "Card not in hand");
        return;
    }

    if (!turnManager->PayEssence(PlayerID, CardData->playCost))
    {
        RejectAction(playerInstigator, "Not enough essence");
        return;
    }

    AAShip* Ship = GetWorld()->SpawnActor<AAShip>(
        ShipClass, FVector::ZeroVector, FRotator::ZeroRotator);

    if (!Ship)
    {
        RejectAction(playerInstigator, "Spawn failed");
        return;
    }

    Ship->CardData = CardData;
    Ship->RuntimeStats = CardData->stats;
    Ship->ownerPlayer = PlayerID;
    boardManager->PlaceShip(Ship, TargetCell);
    deckManager->PlayCard(PlayerID, CardData);

    TArray<UEffect*> RuntimeEffects;
    for (const TObjectPtr<UEffect>& Template : CardData->Effects)
    {
        if (!Template) continue;
        RuntimeEffects.Add(DuplicateObject<UEffect>(Template, Ship));
    }

    for (UEffect* Eff : RuntimeEffects)
    {
        if (IsValid(Eff) && Eff->Trigger == EEffectTrigger::Passive)
        {
            const FCardStats B = Eff->GetPassiveStatBonus();
            Ship->RuntimeStats.firePower += B.firePower;
            Ship->RuntimeStats.maxSpeed += B.maxSpeed;
            Ship->RuntimeStats.resistance += B.resistance;
            Ship->RuntimeStats.radar += B.radar;
            Ship->RuntimeStats.moveCost += B.moveCost;
        }
    }

    Ship->SetHealthPoint(Ship->RuntimeStats.resistance);
    effectManager->RegisterEffects(Ship, RuntimeEffects);
    playerInstigator->ClientOnPlayCard();

    Ship->OnShipSpawn(effectManager->HasHyperspacePilote(boardManager->Motherships[PlayerID])
        || effectManager->HasHyperspace(Ship));

    UpdateGridState();
    BroadcastEssenceChanged(PlayerID);
    SyncHandToPlayer(PlayerID);
}

void AABoardGameMode::HandlePlaceExpert(ABoardPlayerController* playerInstigator, UUCardData* CardData)
{
    if (!ValidateIsPlayerTurn(playerInstigator)) return;

    int32 PlayerID = playerInstigator->PlayerID;

    if (CardData->type != ECardType::EXPERT)
    {
        RejectAction(playerInstigator, "La carte selectionner n'est pas un expert");
        return;
    }

    if (!turnManager->PayEssence(PlayerID, CardData->playCost))
    {
        RejectAction(playerInstigator, "Not enough essence");
        return;
    }

    AAMotherShip* motherShip = boardManager->Motherships[PlayerID];

    if (!IsValid(motherShip))
    {
        RejectAction(playerInstigator, "mothership invalide");
        return;
    }

    if (!motherShip->AddRDCard(CardData))
    {
        RejectAction(playerInstigator, "le vaisseau mère n'a plus de place d'e R&D'expert");
        return;
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("expert placé"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }
    playerInstigator->ClientOnPlayCard();

    TArray<UEffect*> RuntimeEffects;
    for (const TObjectPtr<UEffect>& Template : CardData->Effects)
    {
        if (!Template) continue;
        UEffect* Inst = DuplicateObject<UEffect>(Template, motherShip);
        RuntimeEffects.Add(Inst);
    }
    effectManager->RegisterEffects(motherShip, RuntimeEffects);
}

void AABoardGameMode::HandlePlaceUpgrade(ABoardPlayerController* playerInstigator, UUCardData* CardData, AAShip* ship)
{
    if (!ValidateIsPlayerTurn(playerInstigator)) return;

    int32 PlayerID = playerInstigator->PlayerID;

    if (CardData->type != ECardType::UPGRADE)
    {
        RejectAction(playerInstigator, "La carte selectionner n'est pas une amélioration");
        return;
    }

    if (!CardData->Upgrade.Get()->targetedShipClass.Contains(ship->CardData->shipClass))
    {
        RejectAction(playerInstigator, "le vaisseau n'est pas de la bonne classe");
        return;
    }


    if (!IsValid(ship))
    {
        RejectAction(playerInstigator, "le vaisseau n'est pas valide");
        return;
    }

    if (!turnManager->PayEssence(PlayerID, CardData->playCost))
    {
        RejectAction(playerInstigator, "Not enough essence");
        return;
    }

    CardData->Upgrade->SourceCard = CardData;
    upgradesManager.Get()->AddUpgrade(ship, CardData->Upgrade);
    playerInstigator->ClientOnPlayCard();
}

void AABoardGameMode::HandleRemoveUpgrade(UUpgrade* upgrade, AAShip* ship)
{
    if (!IsValid(ship) || !IsValid(upgrade))
    {
        return;
    }

    upgradesManager.Get()->RemoveUpgrade(ship, upgrade);
}

void AABoardGameMode::HandleTurnStarted(int32 PlayerID, int32 TurnNumber)
{
    BroadcastTurnStarted(PlayerID);
}

void AABoardGameMode::HandleEndTurn(ABoardPlayerController* playerInstigator)
{
    if (!ValidateIsPlayerTurn(playerInstigator)) return;

    int32 CurrentPlayer = playerInstigator->PlayerID;

    turnManager->EndTurn();

    int32 NextPlayer = turnManager->GetNextPlayerId();
    int32 RefineryBonus = boardManager->CheckRefineries(NextPlayer);
    if (RefineryBonus > 0)
        turnManager->AddBonusEssence(NextPlayer, RefineryBonus);

    turnManager->StartTurn(NextPlayer);

    deckManager->DrawCards(NextPlayer, deckManager->DrawPerTurn);

    BroadcastTurnStarted(NextPlayer);
    SyncHandToPlayer(NextPlayer);
}

void AABoardGameMode::HandlePlayCard(
    ABoardPlayerController* playerInstigator,
    UUCardData* Card,
    FIntPoint TargetCell)
{
    if (!ValidateIsPlayerTurn(playerInstigator)) return;

    int32 PlayerID = playerInstigator->PlayerID;

    if (!deckManager->IsCardInHand(PlayerID, Card))
    {
        RejectAction(playerInstigator, "Card not in hand");
        return;
    }

    deckManager->PlayCard(PlayerID, Card);
    SyncHandToPlayer(PlayerID);
}

void AABoardGameMode::BroadcastTurnStarted(int32 PlayerID)
{
    ABoardGameState* GS = GetGameState<ABoardGameState>();
    if (GS)
    {
        GS->SetActivePlayer(
            PlayerID,
            turnManager->GetCurrentTurn(),
            turnManager->GetCurrentTurnPhase()
        );
    }

    for (auto& [ID, PC] : connectedPlayers)
    {
        ABoardPlayerState* PS = PC->GetPlayerState<ABoardPlayerState>();
        if (PS)
        {
            PS->SetEssence(
                turnManager->GetCurrentEssence(ID),
                turnManager->GetMaxEssence(ID),
                turnManager->GetBonusEssence(ID)
            );
            PS->HandCount = deckManager->GetHandCount(ID);
            PS->DeckCount = deckManager->GetDeckCount(ID);
        }

        PC->ClientOnTurnStarted(PlayerID);
    }
}

void AABoardGameMode::BroadcastEssenceChanged(int32 playerId)
{
    TObjectPtr<ABoardPlayerController>* PCPtr = connectedPlayers.Find(playerId);
    if (!PCPtr || !*PCPtr) return;

    ABoardPlayerState* PS = (*PCPtr)->GetPlayerState<ABoardPlayerState>();
    if (!PS) return;

    PS->SetEssence(
        turnManager->GetCurrentEssence(playerId),
        turnManager->GetMaxEssence(playerId),
        turnManager->GetBonusEssence(playerId)
    );
}

void AABoardGameMode::SyncHandToPlayer(int32 PlayerID)
{
    ABoardPlayerController* PC = connectedPlayers.FindRef(PlayerID);
    if (!PC) return;

    const TArray<UUCardData*>& Hand = deckManager->GetHand(PlayerID);

    ABoardPlayerState* PS = PC->GetPlayerState<ABoardPlayerState>();
    if (PS) PS->SetHand(Hand);

    PC->ClientUpdateHand(Hand);

    for (auto& [ID, OtherPC] : connectedPlayers)
    {
        ABoardPlayerState* OtherPS =
            OtherPC->GetPlayerState<ABoardPlayerState>();
        if (OtherPS && ID == PlayerID)
            OtherPS->HandCount = Hand.Num();
    }
}

void AABoardGameMode::UpdateGridState()
{
    ABoardGameState* GS = GetGameState<ABoardGameState>();
    if (!GS) return;

    for (int32 X = 0; X < UUBoardManager::GridWidth; X++)
    {
        for (int32 Y = 0; Y < UUBoardManager::GridHeight + 2; Y++)
        {
            FCell Cell = boardManager->GetCells(X, Y);
            FReplicatedCellState RepCell;
            RepCell.pos = FIntPoint(X, Y);
            RepCell.bHasShip = (Cell.Occupant != nullptr);
            RepCell.OccupantPlayerId = Cell.Occupant
                ? Cell.Occupant->ownerPlayer : -1;

            GS->UpdateCellState(FIntPoint(X, Y), RepCell);
        }
    }
}

void AABoardGameMode::BroadcastFireResult(const FFireResult& Result)
{
    if (!Result.bShipDestroyed) return;

    for (auto& [ID, PC] : connectedPlayers)
    {
        PC->ClientOnShipDestroyed(Result.HitShip);
    }

    Result.HitShip.Get()->Destroy();
}

bool AABoardGameMode::ValidateIsPlayerTurn(
    ABoardPlayerController* playerInstigator) const
{
    if (!turnManager->IsPlayerTurn(playerInstigator->PlayerID))
    {
        RejectAction(playerInstigator, "Not your turn");
        return false;
    }
    return true;
}

bool AABoardGameMode::ValidateShipOwnership(
    ABoardPlayerController* playerInstigator, AAShip* Ship) const
{
    if (!Ship || Ship->ownerPlayer != playerInstigator->PlayerID)
    {
        RejectAction(playerInstigator, "Not your ship");
        return false;
    }
    return true;
}

void AABoardGameMode::RejectAction(
    ABoardPlayerController* playerInstigator, const FString& Reason) const
{
    if (playerInstigator)
        playerInstigator->ClientOnActionRejected(Reason);
}


void AABoardGameMode::CheckVictoryConditions()
{

}

void AABoardGameMode::OnVictoryConditionMet(int32 WinnerPlayerID)
{
    ABoardGameState* GS = GetGameState<ABoardGameState>();
    if (GS) GS->SetWinner(WinnerPlayerID);
    
    for (auto& [ID, PC] : connectedPlayers)
        PC->ClientOnVictory(WinnerPlayerID);
}

void AABoardGameMode::OnRefineryControlChanged(
    AARefinery* Refinery, int32 NewOwnerID)
{
    UpdateGridState();
}

TArray<FIntPoint> AABoardGameMode::GetReachableCellsForShip(
    ABoardPlayerController* playerInstigator, AAShip* Ship)
{
    if (!ValidateShipOwnership(playerInstigator, Ship))
        return {};

    int32 Essence = turnManager->GetAvaliableEssence(playerInstigator->PlayerID);
    TArray<FReachableCell> Reachable =
        boardManager->GetReachableCells(Ship, Essence);

    TArray<FIntPoint> Positions;
    for (const FReachableCell& RC : Reachable)
        Positions.Add(RC.Cell);

    return Positions;
}

void AABoardGameMode::HandleCardDrawn(int32 PlayerId, UUCardData* Card)
{
    TObjectPtr<ABoardPlayerController>* PCPtr = connectedPlayers.Find(PlayerId);
    if (PCPtr && *PCPtr)
    {
        (*PCPtr)->ClientOnCardDrawn(Card);
    }
}

AABoardGameMode::AABoardGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AABoardGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!turnManager) return;

    turnManager->SetTurnTimer(DeltaTime);

    if (turnManager->CurrentTurnTimer <= 0
        && (turnManager->GetCurrentPlayer() == 0 && turnManager->Player1Timer <= 0
        || turnManager->GetCurrentPlayer() == 1 && turnManager->Player2Timer <= 0))
        {
        HandleEndTurn(connectedPlayers.FindRef(turnManager->GetCurrentPlayer()));
        }

    ABoardGameState* GS = GetGameState<ABoardGameState>();

    if (!GS) return;

    GS->CurrentTurnTimer = turnManager->CurrentTurnTimer;
    GS->Player1Timer = turnManager->Player1Timer;
    GS->Player2Timer = turnManager->Player2Timer;
}
