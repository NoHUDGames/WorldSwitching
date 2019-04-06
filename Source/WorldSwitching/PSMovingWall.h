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

	class UTimelineComponent* MyTimeline;

public:
	APSMovingWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		FVector StartLocation;

	UPROPERTY(VisibleAnywhere)
		FVector EndLocation;

	UPROPERTY(EditAnywhere)
		float MovementSpeed;

	UPROPERTY(EditAnywhere)
		float PauseDuration;

	UPROPERTY(EditAnywhere)
		FVector MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ActivateMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool MoveBackAndForth;

	void MovingActorWithReverse(float DeltaTime);
	void MovingActor(float DeltaTime);

	void setIsReverse();

private:
	bool isReverse{ false };
	FTimerHandle PauseTimerHandler;
	bool TimerRunning{ false };
};
