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
#include "Altar.h"
#include "LevelCamera.h"
#include "OurEnums.h"
#include "WorldSwitchingGameInstance.h"
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
	void ChangeWorlds(bool bShowTransitionEffects = true);

	void ToggleAll();

	//Because input cannot trigger functions with parameters?
	void ChangeWorldsProxy();

	//Returns true if collision occurred
	bool TestPhysicalOverlaps();

	//Not going to deny reentry if colliding with Spirit Objects.
	void TestSpiritOverlaps();

	bool bIsSpiritWorld;
	
	
	UCameraComponent* CameraComponent = nullptr;

	ALevelCamera* FreeLevelCamera = nullptr;

	void GatherLevelCameras();

	ALevelCamera* GetFreeLevelCamera() { return FreeLevelCamera; }

	APlayerController* PlayerController = nullptr;

	UCapsuleComponent* PlayerCapsuleCollision = nullptr;

	ABP_Character* PlayerPawn = nullptr;

	AActor* OtherActorPhysicalTest = nullptr;


	UWorldSwitchingGameInstance* GameInstance = nullptr;



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


