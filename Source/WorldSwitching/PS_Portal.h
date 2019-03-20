// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
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
	void TravelSequence(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult &SweepResult);

	void ExitLevel();

	//TIMELINE

	UTimelineComponent* TimeLineMovePortalCamera = nullptr;

	UPROPERTY(EditAnywhere, Category = CameraMovement)
	class UCurveFloat* CameraPortalMovement = nullptr;

	FOnTimelineFloat InterpFunction{};

	UFUNCTION()
	void MoveCameraIntoPortal(float value);
	//TIMELINE/

private:
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

		FRotator CameraPointsLookAt;

public:

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Travel)
		int ArtifactsNeededToUse = 0;

		bool bIsActive = false;

		int GetArtifactsNeededToUse() { return ArtifactsNeededToUse; }

		UPROPERTY(EditAnywhere)
		EPortalIndex PortalIndex;

		void MoveCameraProxy();

};
