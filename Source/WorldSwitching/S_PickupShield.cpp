// Fill out your copyright notice in the Description page of Project Settings.

#include "S_PickupShield.h"

void AS_PickupShield::Tick(float DeltaTime)
{
	AddActorLocalRotation(FRotator(0.f, 50.f, 0.f)*DeltaTime);


}