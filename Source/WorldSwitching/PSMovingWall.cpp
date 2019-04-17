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

	StartLocation = GetRootComponent()->GetRelativeTransform().GetLocation();
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
		
		if(GetRootComponent()->GetRelativeTransform().GetLocation().Equals(EndLocation, 10.f))
		{
			GetRootComponent()->SetRelativeLocation(EndLocation);
			setIsReverse();
			
		}
		else
		{
			GetRootComponent()->SetRelativeLocation(GetRootComponent()->GetRelativeTransform().GetLocation() + (MovementDirection * MovementSpeed * DeltaTime));
		}
	}
	else if (isReverse == true)
	{
		
		if (GetRootComponent()->GetRelativeTransform().GetLocation().Equals(StartLocation, 10.f))
		{
			GetRootComponent()->SetRelativeLocation(StartLocation);
			if (TimerRunning == false)
			{
				TimerRunning = true;
				GetWorldTimerManager().SetTimer(PauseTimerHandler, this, &APSMovingWall::setIsReverse, 1.f, false);
			}
		}
		else
		{
			GetRootComponent()->SetRelativeLocation(GetRootComponent()->GetRelativeTransform().GetLocation() - (MovementDirection * MovementSpeed * DeltaTime));
		}
	}
	
}

void APSMovingWall::MovingActor(float DeltaTime)
{
	if (GetRootComponent()->GetRelativeTransform().GetLocation().Equals(EndLocation, 10.f))
	{
		GetRootComponent()->SetRelativeLocation(EndLocation);
		ActivateMoving = false;
	}

	GetRootComponent()->SetRelativeLocation(GetRootComponent()->GetRelativeTransform().GetLocation() + (MovementDirection * MovementSpeed * DeltaTime));
}

void APSMovingWall::setIsReverse()
{
	isReverse = !isReverse;
	TimerRunning = false;
}


