// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WorldSwitchingGameModeBase.generated.h"

/**
 * 
 */

class UCameraComponent;

UCLASS()
class WORLDSWITCHING_API AWorldSwitchingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

		AWorldSwitchingGameModeBase();

		virtual void BeginPlay() override;
		
public:

	void ChangeWorlds();

	bool bIsSpiritWorld;
	
	
	UCameraComponent* CameraComponent = nullptr;

	APlayerController* PlayerController = nullptr;

	UFUNCTION(BlueprintCallable)
		UCameraComponent* GetCameraComponent()
	{
		return CameraComponent;
	}

	UFUNCTION(BlueprintImplementableEvent)
		void WorldTransitionEffects();
};
