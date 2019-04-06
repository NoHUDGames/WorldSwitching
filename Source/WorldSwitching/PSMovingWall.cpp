// Fill out your copyright notice in the Description page of Project Settings.

#include "PSMovingWall.h"


APSMovingWall::APSMovingWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PauseDuration = 1.f;

	ActivateMoving = true;
	MoveBackAndForth = true;
}

void APSMovingWall::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	
	EndLocation = (StartLocation + MovementDirection);
}

void APSMovingWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ActivateMoving)
	{
		if (MoveBackAndForth)
		{
			MovingActorWithReverse(DeltaTime);
		}
		else
		{
			MovingActor(DeltaTime);
		}
		
	}

}

void APSMovingWall::MovingActorWithReverse(float DeltaTime)
{
	if (isReverse == false)
	{
		

		if (GetActorLocation().Equals(EndLocation, 10.f))
		{
			setIsReverse();
			
		}
		else
		{
			SetActorLocation(GetActorLocation() + (MovementDirection * MovementSpeed * DeltaTime));
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
			SetActorLocation(GetActorLocation() - (MovementDirection * MovementSpeed * DeltaTime));
		}
	}
	
}

void APSMovingWall::MovingActor(float DeltaTime)
{
	if (GetActorLocation().Equals(EndLocation, 10.f))
	{
		ActivateMoving = false;
	}
	SetActorLocation(GetActorLocation() + (MovementDirection * MovementSpeed * DeltaTime));
}

void APSMovingWall::setIsReverse()
{
	isReverse = !isReverse;
	TimerRunning = false;
}


