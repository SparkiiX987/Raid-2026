#pragma once

#include "CoreMinimal.h"
#include "Effect.h"
#include "ActivatedEffect.h"
#include "TriggeredEffect.h"
#include "PassiveEffect.h"
#include "ResearchAndDeveloppement.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API USientistExpert : public UTriggeredEffect
{
	GENERATED_BODY()

public:
	USientistExpert()
	{
		Trigger = EEffectTrigger::OnStartOfTurn;
		DisplayName = FText::FromString(TEXT("Expert scientifique"));
		Description = FText::FromString(FString::Printf(TEXT("Au début de votre tour piocher %d carte"), DrawCount));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 DrawCount = 1;

	virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UAutoRegeneration : public UTriggeredEffect
{
	GENERATED_BODY()

public:
	UAutoRegeneration()
	{
		Trigger = EEffectTrigger::OnStartOfTurn;
		DisplayName = FText::FromString(TEXT("Matière auto régénérante"));
		Description = FText::FromString(FString::Printf(TEXT("Au début de votre tour ajouter +%d de résisrance à tout vos vaisseau (la resistance ne peut pas dépasser celle de base)"), RegenerationAmount));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 RegenerationAmount = 1;

	virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API URaffinageExpert : public UTriggeredEffect
{
	GENERATED_BODY()

public:
	URaffinageExpert()
	{
		Trigger = EEffectTrigger::OnStartOfTurn;
		DisplayName = FText::FromString(TEXT("Expert en rafinage"));
		Description = FText::FromString(FString::Printf(TEXT("Si vous controler la rafinerie, Rafinnage %d début de votre tour"), BonusEssence));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 BonusEssence = 1;

	virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};