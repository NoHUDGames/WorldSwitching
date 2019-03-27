// Fill out your copyright notice in the Description page of Project Settings.

#include "PSMovingWall.h"

APSMovingWall::APSMovingWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APSMovingWall::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	
	MovementDirection = (EndLocation - StartLocation).GetSafeNormal();
}

void APSMovingWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MovingActor();
}

void APSMovingWall::MovingActor()
{
	if (isReverse == false)
	{
		SetActorLocation(GetActorLocation() + (MovementDirection * MovementSpeed));

		if (GetActorLocation().Equals(EndLocation, 1.f))
		{
			isReverse = true;
		}
	}
	else if (isReverse == true)
	{
		SetActorLocation(GetActorLocation() - (MovementDirection * MovementSpeed));

		if (GetActorLocation().Equals(StartLocation, 1.f))
		{
			isReverse = false;
		}
	}
	
}
