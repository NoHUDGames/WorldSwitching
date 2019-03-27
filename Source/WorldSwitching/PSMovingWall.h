// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "Components/TimelineComponent.h"
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

	UPROPERTY(EditAnywhere)
		FVector EndLocation;

	UPROPERTY(EditAnywhere)
		float MovementSpeed;

	UPROPERTY(VisibleAnywhere)
		FVector MovementDirection;

	void MovingActor();

private:
	bool isReverse{ false };
};
