#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	Inactive UMETA(DisplayName="Inactive"),
	StartTurn UMETA(DisplayName="Start Turn"),
	Main UMETA(DisplayName="Main"),
	EndTurn UMETA(DisplayName="End Turn")
};