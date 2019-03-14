// Fill out your copyright notice in the Description page of Project Settings.

#include "OurPlayerController.h"

void AOurPlayerController::DecrementHealth()
{
	--Health;
}

void AOurPlayerController::SetHealth(int NewHealth)
{
	Health = NewHealth;
}

int AOurPlayerController::GetHealth()
{
	return Health;
}

void AOurPlayerController::IncrementArtifacts()
{
	++Artifacts;
}