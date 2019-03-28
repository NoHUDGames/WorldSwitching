// Fill out your copyright notice in the Description page of Project Settings.

#include "PSMovingWall.h"


APSMovingWall::APSMovingWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PauseDuration = 1.f;
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
		

		if (GetActorLocation().Equals(EndLocation, 10.f))
		{
			setIsReverse();
			
		}
		else
		{
			SetActorLocation(GetActorLocation() + (MovementDirection * MovementSpeed));
		}
	}
	else if (isReverse == true)
	{
		

		if (GetActorLocation().Equals(StartLocation, 10.f))
		{
			if (TimerRunning == false)
			{
				TimerRunning = true;
				GetWorldTimerManager().SetTimer(PauseTimerHandler, this, &APSMovingWall::setIsReverse, 1.f, false);
			}
		}
		else
		{
			SetActorLocation(GetActorLocation() - (MovementDirection * MovementSpeed));
		}
			
	}
	
}

void APSMovingWall::setIsReverse()
{
	isReverse = !isReverse;
	TimerRunning = false;
}


