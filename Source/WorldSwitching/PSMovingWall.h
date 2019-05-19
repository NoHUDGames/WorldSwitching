// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "TimerManager.h"
#include "PSMovingWall.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API APSMovingWall : public APSWorldActor
{
	GENERATED_BODY()

public:
	APSMovingWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	///The start location that the wall should move from
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartLocation;

	///The Location where the wall should move to
	UPROPERTY(VisibleAnywhere)
		FVector EndLocation;

	///How quickly the wall moves
	UPROPERTY(EditAnywhere)
		float MovementSpeed;

	///How long the wall stays still at the start location, if it has pause enabled
	UPROPERTY(EditAnywhere)
		float PauseDuration;

	///The direction in which the wall should move
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector MovementDirection;

	///Activates the movement of the wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ActivateMoving;

	///Whether or not the wall should move back and forth
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool MoveBackAndForth;

	///Moves the wall back and forth
	void MovingActorWithReverse(float DeltaTime);

	///Moves the wall in only one direction and makes it stay there
	void MovingActor(float DeltaTime);

	///Reverses the isReverse value (true if false, false if true)
	void setIsReverse();

private:
	///The direction the wall is moving (is it moving backwards of forwards?)
	bool isReverse{ false };

	///Timer handler for the pause time
	FTimerHandle PauseTimerHandler;

	///Whether or not the pause timer is running
	bool TimerRunning{ false };
};
