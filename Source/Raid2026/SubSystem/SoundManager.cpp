// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundManager.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"




//Play sound
void USoundManager::PlaySFX(USoundBase* Sound)
{
	float FinalVolume = MasterVolume * SFXVolume;
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound, FinalVolume);
	}
}

void USoundManager::PlayAmbientSound(USoundBase* Sound)
{
	float FinalVolume = MasterVolume * AmbientVolume;
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound, FinalVolume);
	}
}

void USoundManager::PlayMusic(USoundBase* Sound)
{
	float FinalVolume = MasterVolume * MusicVolume;
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound, FinalVolume);
	}
}

void USoundManager::PlayUISound(USoundBase* Sound)
{
	float FinalVolume = MasterVolume * SFXVolume;
	if (Sound)
	{
		UGameplayStatics::CreateSound2D(GetWorld(), Sound, FinalVolume, 1.0f, 0.0f, nullptr, true, true)->Play();
	}
}

//Volume change
void USoundManager::ChangeVolumeSFX(float NewSFXVolume)
{
	SFXVolume = FMath::Clamp(NewSFXVolume, 0.0f, 1.5f);
}

void USoundManager::ChangeVolumeAmbient(float NewAmbientVolume)
{
	AmbientVolume = FMath::Clamp(NewAmbientVolume, 0.0f, 1.5f);
}

void USoundManager::ChangeVolumeMusic(float NewMusicVolume)
{
	MusicVolume = FMath::Clamp(NewMusicVolume, 0.0f, 1.5f);
}

void USoundManager::ChangeMasterVolume(float NewMasterVolume)
{
	MasterVolume = FMath::Clamp(NewMasterVolume, 0.0f, 1.5f);
}