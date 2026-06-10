#include "ABoardGameMode.h"
#include "../CoreLayer/Cells/ReplicatedCellState.h"
#include "../CoreLayer/Cells/FReachableCell.h"
#include "../GameState/BoardGameState.h"
#include "../PlayerState/BoardPlayerState.h"

void AABoardGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void AABoardGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ABoardPlayerController* PC =
        Cast<ABoardPlayerController>(NewPlayer);
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

void AABoardGameMode::StartGameWhenReady()
{
    if (connectedPlayers.Num() < playerCount) return;

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("GameMode: Player connected : %d"),
            connectedPlayers.Num());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    StartGame();
}

void AABoardGameMode::StartGame()
{
    SpawnManagers();
    SpawnBoardActors();
    SpawnVisualiser();

    turnManager->InitializeGame(0, playerCount);

    for (auto& [ID, PC] : connectedPlayers)
    {
        deckManager->InitializeDeck(ID, testDeck/*TODO changer et récupérer le deck du joueur*/);
    }

    BroadcastTurnStarted(0);
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

    turnManager->boardManager = boardManager;
    turnManager->deckManager = deckManager;
    combatResolver->Board = boardManager;

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
            FString text = FString::Printf(TEXT("SpawnVisualiser: boardVisualiserClass non assignée !"));

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
            FString text = FString::Printf(TEXT("SpawnVisualiser: échec du spawn !"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
        }
        return;
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

    int32 PlayerID = playerInstigator->PlayerID;
    int32 AvailableEssence = turnManager->GetAvaliableEssence(PlayerID);

    TArray<FReachableCell> Reachable =
        boardManager->GetReachableCells(Ship, AvailableEssence);

    const FReachableCell* targetCellData = nullptr;
    bool bIsReachable = Reachable.ContainsByPredicate(
        [&](const FReachableCell& RC) {
            if (RC.Cell == TargetCell)
            {
                targetCellData = &RC;
                return true;
            }

            return false;
        });

    if (!bIsReachable || targetCellData == nullptr)
    {
        RejectAction(playerInstigator, "Cell not reachable");
        return;
    }

    int32 MoveCost = targetCellData->EssenceCost;
    if (!turnManager->PayEssence(PlayerID, MoveCost))
    {
        RejectAction(playerInstigator, "Not enough essence");
        return;
    }

    boardManager->MoveShipTo(Ship, TargetCell, PlayerID);
    if (boardManager->GetCell(TargetCell).Type == ECellType::Refinery)
    {
        boardManager->GetCell(TargetCell).refinery->Capture(Ship->ownerPlayer);
    }

    UpdateGridState();
    BroadcastEssenceChanged(PlayerID);

    for (auto& [ID, PC] : connectedPlayers)
        PC->ClientOnShipMoved(Ship, TargetCell);

    CheckVictoryConditions();
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

    BroadcastFireResult(Result);
    BroadcastEssenceChanged(PlayerID);
    UpdateGridState();
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

    UpdateGridState();
    BroadcastEssenceChanged(PlayerID);
    SyncHandToPlayer(PlayerID);
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

void AABoardGameMode::BroadcastEssenceChanged(int32 PlayerID)
{
    ABoardPlayerController* PC = connectedPlayers.FindRef(PlayerID);
    if (!PC) return;

    ABoardPlayerState* PS = PC->GetPlayerState<ABoardPlayerState>();
    if (PS)
    {
        PS->SetEssence(
            turnManager->GetCurrentEssence(PlayerID),
            turnManager->GetMaxEssence(PlayerID),
            turnManager->GetBonusEssence(PlayerID)
        );
    }

    PC->ClientOnEssenceChanged(
        turnManager->GetCurrentEssence(PlayerID),
        turnManager->GetMaxEssence(PlayerID)
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