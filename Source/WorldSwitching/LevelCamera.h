// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "OurEnums.h"
#include "LevelCamera.generated.h"

/**
 * 
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
