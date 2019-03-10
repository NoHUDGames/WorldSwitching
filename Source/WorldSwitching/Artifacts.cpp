// Fill out your copyright notice in the Description page of Project Settings.

#include "Artifacts.h"

AArtifacts::AArtifacts()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AArtifacts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.f, 50.f, 0.f)*DeltaTime);

}

bool AArtifacts::GetHasBeenPickedUp()
{
	return bHasBeenPickedUp;
}
