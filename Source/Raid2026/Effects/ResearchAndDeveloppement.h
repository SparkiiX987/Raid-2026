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

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UArtillerySpecialist : public UTriggeredEffect
{
	GENERATED_BODY()

public:
	UArtillerySpecialist()
	{
		Trigger = EEffectTrigger::OnStartOfTurn;
		DisplayName = FText::FromString(TEXT("Artilleur Spécialisé"));
		Description = FText::FromString(FString::Printf(TEXT("A partir du prochain tour où vous jouer cette carte, le Canon a Ion inflige %d dégat a un vaisseau mére adverse, un tour sur deux"), damages));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 damages = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 delay = 1;

	UPROPERTY()
	int32 currentDelay = 0;

	virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UHyperspacePilote : public UPassiveEffect
{
	GENERATED_BODY()

public:
	UHyperspacePilote()
	{
		Trigger = EEffectTrigger::Passive;
		DisplayName = FText::FromString(TEXT("Pilote d'Hyperespace"));
		Description = FText::FromString(FString::Printf(TEXT("Tous vos vaisseau gagne Hyperespace")));
	}

	virtual FEffectResult Apply_Implementation(const FEffectContext& Context) override;
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class RAID2026_API UBiocarburantExpert : public UPassiveEffect
{
	GENERATED_BODY()

public:
	UBiocarburantExpert()
	{
		Trigger = EEffectTrigger::Passive;
		DisplayName = FText::FromString(TEXT("Expert en BioCarburant"));
		Description = FText::FromString(FString::Printf(TEXT("Les vaisseau de classe 4 ou plus on un cout de déplacement reduit de %d"), BonusEssence));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 BonusEssence = 1;
};