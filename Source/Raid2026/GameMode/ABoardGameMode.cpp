#include "ABoardGameMode.h"
#include "../CoreLayer/Cells/ReplicatedCellState.h"
#include "../CoreLayer/Cells/FReachableCell.h"
#include "../GameState/BoardGameState.h"
#include "../PlayerState/BoardPlayerState.h"
#include <Raid2026/Effects/Effect.h>

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
        deckManager->InitializeDeck(ID, testDeck/*TODO changer et r�cup�rer le deck du joueur*/);
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
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("in HandleSpawnShip"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    if (!ValidateIsPlayerTurn(playerInstigator)) return;

    int32 PlayerID = playerInstigator->PlayerID;

    if (!boardManager->GetFreeSpawnCells(PlayerID).Contains(TargetCell))
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("cell pas free"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
        RejectAction(playerInstigator, "Invalid spawn cell");
        return;
    }

    if (!deckManager->IsCardInHand(PlayerID, CardData))
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("carte pas en main"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
        RejectAction(playerInstigator, "Card not in hand");
        return;
    }

    if (!turnManager->PayEssence(PlayerID, CardData->stats.spawnCost))
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("pas assez de tune"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
        RejectAction(playerInstigator, "Not enough essence");
        return;
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("spawn ship"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    AAShip* Ship = GetWorld()->SpawnActor<AAShip>(
        ShipClass, FVector::ZeroVector, FRotator::ZeroRotator);

    if (!Ship)
    {
        RejectAction(playerInstigator, "Spawn failed");
        return;
    }

    Ship->CardData = CardData;
    Ship->ownerPlayer = PlayerID;
    boardManager->PlaceShip(Ship, TargetCell);
    deckManager->PlayCard(PlayerID, CardData);
    Ship->SetHealthPoint(CardData->stats.resistance);
    Ship->OnShipSpawn();
    playerInstigator->OnCardPlayedBP();

    TArray<UEffect*> RuntimeEffects;
    for (const TObjectPtr<UEffect>& Template : CardData->Effects)
    {
        if (!Template) continue;
        UEffect* Inst = DuplicateObject<UEffect>(Template, Ship);
        RuntimeEffects.Add(Inst);
    }
    effectManager->RegisterShipEffects(Ship, RuntimeEffects);

    UpdateGridState();
    BroadcastEssenceChanged(PlayerID);
    SyncHandToPlayer(PlayerID);
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
