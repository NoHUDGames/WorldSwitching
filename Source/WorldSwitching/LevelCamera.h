// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "OurEnums.h"
#include "LevelCamera.generated.h"

/**
 *  This is an actor with a camera component (in the blueprint), called BP_LevelCamera, that is used for 
 doing all the portal effects, and in cases where we switch between cameras in a level
 */
UCLASS()
class WORLDSWITCHING_API ALevelCamera : public ACameraActor
{

public:
	UPROPERTY(EditAnywhere)
	ECameraActor CameraType;

	GENERATED_BODY()

		virtual void BeginPlay() override;
};
