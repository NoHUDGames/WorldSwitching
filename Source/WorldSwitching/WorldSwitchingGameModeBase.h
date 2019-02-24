// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "BP_Character.h"
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

	UFUNCTION(BlueprintCallable)
	void ChangeWorlds();

	//Returns true if collision occurred
	bool TestPhysicalCollision();

	bool bIsSpiritWorld;
	
	
	UCameraComponent* CameraComponent = nullptr;

	APlayerController* PlayerController = nullptr;

	UCapsuleComponent* PlayerCapsuleCollision = nullptr;

	ABP_Character* PlayerPawn = nullptr;

	AActor* OtherActorPhysicalTest = nullptr;

	UPROPERTY(EditAnywhere, Category ="Error Sound")
	USoundBase* CannotReturnToPhysical = nullptr;

	UFUNCTION(BlueprintCallable)
		UCameraComponent* GetPawnCameraComponent()
	{
		return CameraComponent;
	}

	//Gir ut actoren man kolliderte med når 
	UFUNCTION(BlueprintCallable)
		AActor* GetOtherActorPhysicalTest();

	UFUNCTION(BlueprintImplementableEvent)
		void LightUpCollidingActor();

	UFUNCTION(BlueprintImplementableEvent)
	void WorldTransitionEffects();

	UFUNCTION(BlueprintImplementableEvent)
	void DeniedPhysicalReentryEffects();

	void TogglePhysicalWorldActors();
	void ToggleSpiritWorldActors();
	void ToggleLastingCameraEffects();

	//For ParticleEffects that have a spiritworld and physicalworld component
	void ToggleParticleEffects();

	void TogglePhysicalSpiritMaterialProperties();
};


