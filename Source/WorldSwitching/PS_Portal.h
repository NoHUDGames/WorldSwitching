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
#include "Camera/CameraActor.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "PS_Portal.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API APS_Portal : public APSWorldActor
{
	GENERATED_BODY()


		APS_Portal();

public:

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void Activate(bool WithOpeningSequence = false);

	FVector CameraOuterLocation;
	FVector CameraInnerLocation;

	UFUNCTION()
	void TravelExitSequence(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult &SweepResult);

	void ExitLevel();

	//TIMELINE

	UTimelineComponent* TimeLineMovePortalCamera = nullptr;
	UTimelineComponent* TimeLineMovePortalPlayer = nullptr;

	UPROPERTY(EditAnywhere, Category = CameraMovement)
	class UCurveFloat* CameraPortalMovement = nullptr;

	FOnTimelineFloat InterpFunction1{};
	FOnTimelineFloat InterpFunction2{};

	UFUNCTION()
	void MoveCameraIntoPortal(float value);

	UFUNCTION()
	void MovePlayerIntoPortal(float value);
	//TIMELINE/


		UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh2 = nullptr;

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

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPointLightComponent* PortalLight = nullptr;

		UPROPERTY(EditAnywhere, Category = "ParticleEffectToSpawn")
		TSubclassOf<AParticleEffectActor> ParticleEffectToSpawn;

		UPROPERTY(EditAnywhere, Category = Travel)
		EDestinationLevel DestinationLevel;

		UPROPERTY(EditAnywhere, Category = Travel)
		bool bBeginActivated = false;

		


		ABP_Character* PlayerPawn = nullptr;

		UWorldSwitchingGameInstance* GameInstance = nullptr;

		AWorldSwitchingGameModeBase* GameMode = nullptr;

		ACameraActor* LevelCamera = nullptr;

		FTimerHandle ExitHandle;
		FTimerHandle CameraMoveHandle;
		FTimerHandle CameraFadeHandle;

		FRotator CameraPointsLookAt;



		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Travel)
		int ArtifactsNeededToUse = 0;

		bool bIsActive = false;
		float LightIntensity = 15000;

		//True for In, False for out
		bool bCameraFadeInOut = false;

		int GetArtifactsNeededToUse() { return ArtifactsNeededToUse; }

		UPROPERTY(EditAnywhere)
		EPortalIndex PortalIndex;

		void MoveCameraProxy();

		UFUNCTION(BlueprintImplementableEvent)
		void CameraFadeInOut(ACameraActor* LevelCamera, bool FadeInOut);

		void CameraFadeProxy();



};
