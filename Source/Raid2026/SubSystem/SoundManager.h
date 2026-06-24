// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundManager.generated.h"

/**
 * 
 */
UCLASS()
class RAID2026_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public :

	UFUNCTION(BlueprintCallable)
	void PlaySFX(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void PlayAmbientSound(USoundBase* Sound);
	
	UFUNCTION(BlueprintCallable)
	void PlayMusic(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void PlayUISound(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void ChangeVolumeSFX(float NewSFXVolume);
	
	UFUNCTION(BlueprintCallable)
	void ChangeVolumeAmbient(float NewAmbientVolume);

	UFUNCTION(BlueprintCallable)
	void ChangeVolumeMusic(float NewMusicVolume);

	UFUNCTION(BlueprintCallable)
	void ChangeMasterVolume(float NewMasterVolume);


protected:
	float MasterVolume;
	float SFXVolume;
	float AmbientVolume;
	float MusicVolume;


};
