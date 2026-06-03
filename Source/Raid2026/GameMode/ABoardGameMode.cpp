#include "ABoardGameMode.h"

UUBoardManager* AABoardGameMode::BoardManagerGet() const
{
    return boardManager.Get();
}

UUTurnManager* AABoardGameMode::GetTurnManager() const
{
    return turnManager.Get();
}

UDeckManager* AABoardGameMode::GetDeckManager() const
{
    return deckManager.Get();
}

UCombatResolver* AABoardGameMode::GetCombatResolver() const
{
    return combatResolver.Get();
}

void AABoardGameMode::SpawnManagers()
{
    if (boardManagerClass)
    {
        boardManager = NewObject<UUBoardManager>(
            this,
            boardManagerClass
        );
    }

    if (turnManagerClass)
    {
        turnManager = NewObject<UUTurnManager>(
            this,
            turnManagerClass
        );
    }

    if (deckManagerClass)
    {
        deckManager = NewObject<UDeckManager>(
            this,
            deckManagerClass
        );
    }

    if (combatResolverClass)
    {
        combatResolver = NewObject<UCombatResolver>(
            this,
            combatResolverClass
        );
    }
}
