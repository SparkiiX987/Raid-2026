#include "BoardPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABoardPlayerState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABoardPlayerState, CurrentEssence);
    DOREPLIFETIME(ABoardPlayerState, MaxEssence);
    DOREPLIFETIME(ABoardPlayerState, BonusEssence);
    DOREPLIFETIME(ABoardPlayerState, HandCount);
    DOREPLIFETIME(ABoardPlayerState, DeckCount);
    DOREPLIFETIME(ABoardPlayerState, id);

    DOREPLIFETIME_CONDITION(ABoardPlayerState, Hand, COND_OwnerOnly);
}

void ABoardPlayerState::SetPlayerId(int32 ID)
{
    id = ID;
}

void ABoardPlayerState::SetEssence(
    int32 Current, int32 Max, int32 Bonus)
{
    CurrentEssence = Current;
    MaxEssence = Max;
    BonusEssence = Bonus;
}

void ABoardPlayerState::SetHand(const TArray<UUCardData*>& NewHand)
{
    Hand = NewHand;
    HandCount = NewHand.Num();
}

void ABoardPlayerState::OnRep_Essence()
{
    OnEssenceChanged.Broadcast(CurrentEssence, MaxEssence);
}

void ABoardPlayerState::OnRep_Hand()
{
    OnHandChanged.Broadcast(Hand);
}

void ABoardPlayerState::OnRep_HandCount()
{
    OnHandCountChanged.Broadcast(HandCount);
}