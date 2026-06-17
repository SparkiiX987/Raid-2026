#include "FEffectResult.h"

FEffectResult FEffectResult::Success()
{
    FEffectResult result = FEffectResult();
    result.bSuccess = true;

    return result;
}

FEffectResult FEffectResult::Fail(const FString& Reason)
{
    FEffectResult result = FEffectResult();
    result.bSuccess = false;

    if (GEngine)
    {
        FString text = FString::Printf(TEXT("Effect fail ! "));
        text += Reason;

        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, text);
    }

    return result;
}

FEffectResult FEffectResult::NeedsTarget()
{
    FEffectResult result = FEffectResult();
    result.bNeedsTarget = true;

    return result;
}
