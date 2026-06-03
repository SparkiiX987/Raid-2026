#include "FEffectResult.h"

FEffectResult FEffectResult::Success()
{
    return FEffectResult();
}

FEffectResult FEffectResult::Fail(const FString& Reason)
{
    return FEffectResult();
}

FEffectResult FEffectResult::NeedsTarget()
{
    return FEffectResult();
}
