#include "Effect.h"

FEffectResult UEffect::Apply_Implementation(const FEffectContext& Context)
{
    if (GEngine)
    {
        FString text = FString::Printf(
            TEXT("UEffect::Apply appelé sur la classe de base (%s) — surcharger dans la sous-classe"),
            *GetDebugName());

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    return FEffectResult::Fail(TEXT("Effet non implemente"));
}

bool UEffect::CanApply_Implementation(const FEffectContext& Context) const
{
    return true;
}

FCardStats UEffect::GetPassiveStatBonus_Implementation() const
{
    return FCardStats{};
}
