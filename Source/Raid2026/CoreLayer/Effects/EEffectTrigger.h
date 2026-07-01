#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEffectTrigger : uint8
{
	OnReveal         UMETA(DisplayName="On Reveal"),
	OnEnterPlay      UMETA(DisplayName="On Enter Play"),
	OnDestroyed      UMETA(DisplayName="On Destroyed"),
	OnStartOfTurn    UMETA(DisplayName="On Start Of Turn"),
	OnEndOfTurn      UMETA(DisplayName="On End Of Turn"),
	OnAllyEntersPlay UMETA(DisplayName="On Ally Enters Play"),
	OnEnemyEntersPlay UMETA(DisplayName="On Enemy Enters Play"),
	OnDamageTaken    UMETA(DisplayName="On Damage Taken"),
	OnRefineryCapture UMETA(DisplayName="On Refinery Capture"),
	OnPlayCard      UMETA(DisplayName="On Play Card"),
	OnMoveShip     UMETA(DisplayName="On Move Ship"),
	Activated        UMETA(DisplayName="Activated"),
	Passive          UMETA(DisplayName="Passive")
};