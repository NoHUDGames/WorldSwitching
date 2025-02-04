// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PointLightComponent.h"
#include "ParticleEffectActor.h"
#include "BP_Character.h"
#include "WorldSwitchingGameInstance.h"
#include "WorldSwitchingGameModeBase.h"
#include "LevelCamera.h"
#include "Camera/CameraComponent.h"
#include "Curves/CurveFloat.h"
#include "Sound/SoundBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "PS_Portal.generated.h"

/**
 * 
 */

UENUM()

enum EComingOrGoing
{
	COMING,
	GOING
};

UCLASS()
class WORLDSWITCHING_API APS_Portal : public APSWorldActor
{
	GENERATED_BODY()


		APS_Portal();

public:

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* PortalMesh = nullptr;

		UPROPERTY(EditAnywhere)
		UBoxComponent* BoxTrigger = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* CharacterInner = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* CharacterOuter = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* CameraInner = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* CameraOuter = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* EnterCameraInner = nullptr;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPointLightComponent* PortalLight = nullptr;

		UPROPERTY(EditAnywhere)
		USoundBase* PortalEnterSound = nullptr;

		UPROPERTY(EditAnywhere, Category = Animation)
		UAnimSequence* OpenPortal = nullptr;


		//I did this before I learned about ChildActorComponents. That would have been better
		UPROPERTY(EditAnywhere, Category = "ParticleEffectToSpawn")
		TSubclassOf<AParticleEffectActor> ParticleEffectToSpawn;

		//Used to determine which level to open
		UPROPERTY(EditAnywhere, Category = Travel)
		EDestinationLevel DestinationLevel;

		UPROPERTY(EditAnywhere, Category = Travel)
		bool bBeginActivated = false;

		ABP_Character* PlayerPawn = nullptr;
		UWorldSwitchingGameInstance* GameInstance = nullptr;
		AWorldSwitchingGameModeBase* GameMode = nullptr;
		ALevelCamera* LevelCamera = nullptr;
		UCameraComponent* PlayerCamera = nullptr;

		//Used to determine location points to lerp player and camera between during travel sequence
		static EComingOrGoing ComingOrGoing;

		FTimerHandle ExitHandle;
		FTimerHandle PlayerMoveHandle;
		FTimerHandle CameraFadeHandle;
		FTimerHandle ActivatePlayerHandle;

		FRotator PlayerLookAt;


		//Used for portal travel sequences
		FVector CameraInnerLoc;
		FVector CameraOuterLoc;
		FVector CharacterInnerLoc;
		FVector CharacterOuterLoc;
		FVector EnterCameraInnerLoc;


		

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Travel)
		int ArtifactsNeededToUse = 0;

		bool bIsActive = false;
		float LightIntensity = 15000;


		UPROPERTY(EditAnywhere)
		bool bIsMainMenuPortal = false;

		int GetArtifactsNeededToUse() { return ArtifactsNeededToUse; }

		UPROPERTY(EditAnywhere)
		EPortalIndex PortalIndex;

		//used to determine which portal to enter hub level from
		static EPortalIndex PlayerCameFrom;

		//The portal that has valid pointer gets to run the enter level sequence
		APS_Portal* PortalToEnterFrom;

		FString CurrentMapName;

		//Starts the sequence when player overlaps collider
		UFUNCTION()
			void ExitLevelSequence(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
				UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
				bool bFromSweep, const FHitResult &SweepResult);


		//We use this alot for quick use of timeline function in blueprint. C++ version is a big headache,
		//but we have done that also elsewhere
		UFUNCTION(BlueprintImplementableEvent)
		void TL_TriggerEnterLevelSequence();

		UFUNCTION(BlueprintCallable)
		void TL_EnterLevelSequence(float MoveCamera, float MovePlayer, float FadeCamera );


		//InOrOut = True: In, False: Out
		UFUNCTION(BlueprintImplementableEvent)
		void FadeCamera(bool InOrOut, ALevelCamera* LevelFreeCamera);

		UFUNCTION(BlueprintImplementableEvent)
			void ActivationMoveCamera_TL();

	
			void ActivationCameraSequence();

		UFUNCTION(BlueprintCallable)
			void ActivationMoveCamera(float TimeLine);

		void FadeCameraProxy();


		UFUNCTION(BlueprintImplementableEvent)
		void TL_TriggerMovePlayerIntoPortal();

		UFUNCTION(BlueprintCallable)
		void TL_MovePlayerIntoPortal(float MovePlayer, float MoveCamera);
	
		//Opens portal doors, turns on lights and smoke and sets collider to overlap player
		UFUNCTION(BlueprintCallable)
		void Activate(bool bWithOpeningSound = false);

		UPROPERTY(EditAnywhere, Category = Sound)
		USoundBase* OpeningSound = nullptr;

		void ExitLevel();

		bool SetPortalToEnterFrom();

		void SetLevelCamera();

		void SetupNavigationPoints();

		void PreparePlayerAndCamera();

		void ActivatePlayerAfterEntry();
		
};
