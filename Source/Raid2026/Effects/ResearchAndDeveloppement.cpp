#include "ResearchAndDeveloppement.h"

FEffectResult USientistExpert::Apply_Implementation(const FEffectContext& Context)
{
	Context.Deck.Get()->DrawCards(Context.OwnerPlayerID, DrawCount);

	return FEffectResult::Success();
} 
FEffectResult UAutoRegeneration::Apply_Implementation(const FEffectContext& Context)
{
	UUBoardManager* boardManager = Context.Board.Get();

    for (int32 X = 0; X < UUBoardManager::GridWidth; ++X)
        for (int32 Y = 0; Y < UUBoardManager::GridHeight + 2; ++Y)
        {
            AAShip* Ship = boardManager->GetShipAt(FIntPoint(X, Y));
            if (IsValid(Ship) && Ship->GetOwnerID() == Context.OwnerPlayerID)
            {
                Ship->Heal(RegenerationAmount);
            }
        }

	return FEffectResult::Success();
}

FEffectResult URaffinageExpert::Apply_Implementation(const FEffectContext& Context)
{
    Context.Turn.Get()->AddBonusEssence(Context.OwnerPlayerID, BonusEssence);

    return FEffectResult::Success();
}
