#include "BoardGameState.h"
#include "Net/UnrealNetwork.h"
#include "../Subsystem/UBoardManager.h"
#include "../CoreLayer/Cells/ReplicatedCellState.h"

void ABoardGameState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABoardGameState, ActivePlayerID);
    DOREPLIFETIME(ABoardGameState, TurnNumber);
    DOREPLIFETIME(ABoardGameState, CurrentPhase);
    DOREPLIFETIME(ABoardGameState, GridState);
    DOREPLIFETIME(ABoardGameState, WinnerId);
    DOREPLIFETIME(ABoardGameState, CurrentTurnTimer);
    DOREPLIFETIME(ABoardGameState, Player1Timer);
    DOREPLIFETIME(ABoardGameState, Player2Timer);
}

void ABoardGameState::BeginPlay()
{
    Super::BeginPlay();

    int32 TotalCells =
        UUBoardManager::GridWidth * (UUBoardManager::GridHeight + 2);

    GridState.SetNum(TotalCells);

    for (int32 X = 0; X < UUBoardManager::GridWidth; X++)
    {
        for (int32 Y = 0; Y < UUBoardManager::GridHeight + 2; Y++)
        {
            int32 Index = CellIndex(X, Y);
            GridState[Index].pos = FIntPoint(X, Y);
        }
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Grid initialized"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
}

void ABoardGameState::SetActivePlayer(
    int32 PlayerID, int32 Turn, ETurnPhase Phase)
{
    ActivePlayerID = PlayerID;
    TurnNumber = Turn;
    CurrentPhase = Phase;
}

void ABoardGameState::UpdateCellState(
    FIntPoint Pos, const FReplicatedCellState& NewState)
{
    int32 Index = CellIndex(Pos.X, Pos.Y);
    if (GridState.IsValidIndex(Index))
        GridState[Index] = NewState;
}

void ABoardGameState::SetWinner(int32 PlayerID)
{
    WinnerId = PlayerID;
}

void ABoardGameState::OnRep_ActivePlayer()
{
    OnActivePlayerChanged.Broadcast(ActivePlayerID);
}

void ABoardGameState::OnRep_GridState()
{
    OnGridStateChanged.Broadcast(GridState);
}

void ABoardGameState::OnRep_TurnPhase()
{
    OnTurnPhaseChanged.Broadcast(CurrentPhase);
}

void ABoardGameState::OnRep_WinnerID()
{
    if (WinnerId != -1)
        OnGameOver.Broadcast(WinnerId);
}

int32 ABoardGameState::CellIndex(int32 X, int32 Y) const
{
    return X * (UUBoardManager::GridHeight + 2) + Y;
}

FReplicatedCellState ABoardGameState::GetCellState(FIntPoint Pos) const
{
    int32 Index = CellIndex(Pos.X, Pos.Y);
    if (GridState.IsValidIndex(Index))
        return GridState[Index];

    return FReplicatedCellState();
}