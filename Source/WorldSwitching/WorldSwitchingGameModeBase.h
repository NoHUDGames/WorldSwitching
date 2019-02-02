// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Camera/CameraActor.h"
#include "WorldSwitchingGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class WORLDSWITCHING_API AWorldSwitchingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
		virtual void BeginPlay() override;

	
	ACameraActor* PawnCamera;
};
