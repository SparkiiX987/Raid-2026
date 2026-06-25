#include "ConcreteSabotage.h"

FEffectResult UIEMAttack::Apply_Implementation(const FEffectContext& Context)
{
	if (!Context.TargetShip.IsValid())
	{
		return FEffectResult::Fail(TEXT("IEMAttaque : target invalide"));
	}

	if (AAShip* TargetShip = Cast<AAShip>(Context.TargetShip.Get()))
	{
		TargetShip->StartParalize();
		return FEffectResult::Success();
	}

	return FEffectResult::Fail(TEXT("IEMAttaque : target invalide"));
}

FEffectResult UImplementedSabotage::Apply_Implementation(const FEffectContext& Context)
{
    if (AAShip* Ship = Cast<AAShip>(Context.TargetShip.Get()))
    {
        if (Ship->upgrades.IsEmpty())
            return FEffectResult::Fail(TEXT("La cible n'a aucune amélioration"));

        if (!IsValid(Context.targetedUpgrade.Get()))
        {
            FEffectResult r = FEffectResult::NeedsTarget();
            for (UUpgrade* Up : Ship->upgrades)
                r.AffectedCards.Add(Up ? Up->SourceCard : nullptr);
            return r;
        }

        Ship->ServerRemoveUpgrade(Context.targetedUpgrade.Get());
        return FEffectResult::Success();
    }

    if (AAMotherShip* MS = Context.TargetMothership.Get())
    {
        if (MS->RDCards.IsEmpty())
            return FEffectResult::Fail(TEXT("La cible n'a aucune carte expert"));

        if (!IsValid(Context.targetedExpert.Get()))
        {
            FEffectResult r = FEffectResult::NeedsTarget();
            for (UUCardData* C : MS->RDCards)
                r.AffectedCards.Add(C);
            return r;
        }

        UEffect* TargetExpert = Context.targetedExpert.Get()->Effects[0];
        const int32 idx = MS->RDCards.IndexOfByPredicate(
            [TargetExpert](const UUCardData* C) { return C && C->Sabotage == TargetExpert; });
        if (idx != INDEX_NONE) { MS->RemoveRDCard(idx); return FEffectResult::Success(); }
        return FEffectResult::Fail(TEXT("Carte expert introuvable"));
    }

    return FEffectResult::Fail(TEXT("Aucune cible valide"));
}

FEffectResult UEmperorGift::Apply_Implementation(const FEffectContext& Context)
{
    if (!IsValid(Context.Deck) || !IsValid(Context.Turn))
        return FEffectResult::Fail(TEXT("EmperorGift : managers invalides"));

    UDeckManager* Deck = Context.Deck.Get();
    Deck->DrawCards(Context.OwnerPlayerID, 1);

    const int32 Opponent = Context.Turn->GetOpponent(Context.OwnerPlayerID);
    const TArray<UUCardData*>& Hand = Deck->GetHand(Opponent);
    if (Hand.Num() > 0)
        Deck->DiscardCard(Opponent, Hand[FMath::RandRange(0, Hand.Num() - 1)]);

    return FEffectResult::Success();
}

FEffectResult UEssenceExplosion::Apply_Implementation(const FEffectContext& Context)
{
    if (!IsValid(Context.Turn))
        return FEffectResult::Fail(TEXT("Essence explosion : turnManager non valid !"));

    const int32 targetedPlayer = Context.Turn->GetOpponent(Context.OwnerPlayerID);
    Context.Turn->RemoveAllBonusEssence(targetedPlayer);
    return FEffectResult::Success();
}
