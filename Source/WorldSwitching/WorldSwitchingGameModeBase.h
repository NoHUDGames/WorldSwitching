// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"
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

		virtual void Tick(float DeltaTime) override;
		
public:

	void ChangeWorlds();

	bool bIsSpiritWorld;
	
	
	UCameraComponent* CameraComponent = nullptr;

	APlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere)
		USoundBase* SoundToSpiritWorld = nullptr;

	UPROPERTY(EditAnywhere)
		USoundBase* SoundFromSpiritWorld = nullptr;

	//Trenger en liten lag for at spirits ikke skal kollidere med fysiske omgivelser
	//ved skifte av verden

	FTimerHandle SwitchLag;

	UFUNCTION(BlueprintCallable)
		UCameraComponent* GetPawnCameraComponent()
	{
		return CameraComponent;
	}

	UFUNCTION(BlueprintImplementableEvent)
	void WorldTransitionEffects();

	void TogglePhysicalWorldActors();
	void ToggleSpiritWorldActors();
	void ToggleLastingCameraEffects();

	//For ParticleEffects that have a spiritworld and physicalworld component
	void ToggleParticleEffects();
};


