#include "BoardPlayerState.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>
#include <Raid2026/PlayerController/BoardPlayerController.h>

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

void ABoardPlayerState::UpdateEssenceUi()
{
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Try"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner());

    if (!IsValid(PC))
    {
        return;
    }

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("PC trouver"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }

    if (ABoardPlayerController* BPC = Cast<ABoardPlayerController>(PC))
    {
        if (GEngine)
        {
            FString text = FString::Printf(TEXT("PC valid et bien caster"));

            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
        }

        BPC->ClientOnEssenceChanged(CurrentEssence, MaxEssence);
    }
}

void ABoardPlayerState::SetEssence(int32 Current, int32 Max, int32 Bonus)
{
    CurrentEssence = Current;
    MaxEssence = Max;
    BonusEssence = Bonus;

    UpdateEssenceUi();
}

void ABoardPlayerState::SetHand(const TArray<UUCardData*>& NewHand)
{
    Hand = NewHand;
    HandCount = NewHand.Num();
}

void ABoardPlayerState::OnRep_Essence()
{
    if (GEngine)
    {
        FString text = FString::Printf(TEXT("rep essence"));

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
    }
    OnEssenceChanged.Broadcast(CurrentEssence, MaxEssence);

    UpdateEssenceUi();
}

void ABoardPlayerState::OnRep_Hand()
{
    OnHandChanged.Broadcast(Hand);
}

void ABoardPlayerState::OnRep_HandCount()
{
    OnHandCountChanged.Broadcast(HandCount);
}