#include "ResearchAndDeveloppement.h"
#include "../SubSystem/CombatResolver.h"

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

FEffectResult UHyperspacePilote::Apply_Implementation(const FEffectContext& Context)
{
    return FEffectResult::Success();
}

FEffectResult UArtillerySpecialist::Apply_Implementation(const FEffectContext& Context)
{
    int32 ennemyId = Context.OwnerPlayerID == 0 ? 1 : 0;

    currentDelay++;

    if (currentDelay >= delay)
    {
        currentDelay = 0;
        Context.Resolver.Get()->ApplyDamageToMothership(Context.Board.Get()->Motherships[ennemyId], damages);
    }

    return FEffectResult::Success();
}
