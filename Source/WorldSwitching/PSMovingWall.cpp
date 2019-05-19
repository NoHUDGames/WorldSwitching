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

	///Sets the correct location values when the game starts
	StartLocation = GetRootComponent()->GetRelativeTransform().GetLocation();
	EndLocation = (StartLocation + MovementDirection);
}

void APSMovingWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	///makes sure that the wall only moves when it is activated
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

///Moves the wall back and forth
void APSMovingWall::MovingActorWithReverse(float DeltaTime)
{
	if (isReverse == false)
	{
		///Sets the wall to the end location when it is close to it
		///This is to make sure that the wall isn't floating off into eternity, which it has done a couple of times when testing the game without this
		if(GetRootComponent()->GetRelativeTransform().GetLocation().Equals(EndLocation, 10.f))
		{
			GetRootComponent()->SetRelativeLocation(EndLocation);
			setIsReverse();
			
		}
		else
		{
			/// A self-created lerp-lookalike calculation that works pretty well
			///Smoothly moves the wall towards the end location
			GetRootComponent()->SetRelativeLocation(GetRootComponent()->GetRelativeTransform().GetLocation() + (MovementDirection * MovementSpeed * DeltaTime));
		}
	}
	else if (isReverse == true)
	{
		///Sets the wall to the start location when it is close to it
		///This is to make sure that the wall isn't floating off into eternity, which it has done a couple of times when testing the game without this
		if (GetRootComponent()->GetRelativeTransform().GetLocation().Equals(StartLocation, 10.f))
		{
			GetRootComponent()->SetRelativeLocation(StartLocation);
			if (TimerRunning == false)
			{
				TimerRunning = true;
				GetWorldTimerManager().SetTimer(PauseTimerHandler, this, &APSMovingWall::setIsReverse, PauseDuration, false);
			}
		}
		else
		{
			/// A self-created lerp-lookalike calculation that works pretty well
			///Smoothly moves the wall towards the start location
			GetRootComponent()->SetRelativeLocation(GetRootComponent()->GetRelativeTransform().GetLocation() - (MovementDirection * MovementSpeed * DeltaTime));
		}
	}
	
}

///Moves the actor in only one direction, and not back again
void APSMovingWall::MovingActor(float DeltaTime)
{
	///Sets the wall to the end location when it is close to it
	///This is to make sure that the wall isn't floating off into eternity, which it has done a couple of times when testing the game without this
	if (GetRootComponent()->GetRelativeTransform().GetLocation().Equals(EndLocation, 10.f))
	{
		GetRootComponent()->SetRelativeLocation(EndLocation);
		ActivateMoving = false;
	}

	/// A self-created lerp-lookalike calculation that works pretty well
	///Smoothly moves the wall towards the end location
	GetRootComponent()->SetRelativeLocation(GetRootComponent()->GetRelativeTransform().GetLocation() + (MovementDirection * MovementSpeed * DeltaTime));
}

///reverses the isReverse boolean value
void APSMovingWall::setIsReverse()
{
	isReverse = !isReverse;
	TimerRunning = false;
}


