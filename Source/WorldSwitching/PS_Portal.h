// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "ParticleEffectActor.h"
#include "OurEnums.h"
#include "BP_Character.h"
#include "WorldSwitchingGameInstance.h"
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

	void ActivatePortal();


	UFUNCTION()
	void Travel(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult &SweepResult);

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

		UPROPERTY(EditAnywhere)
		EDestinationLevel DestinationLevel;

		UPROPERTY(EditAnywhere)
		bool bBeginActivated = false;

		ABP_Character* PlayerPawn = nullptr;

		UWorldSwitchingGameInstance* GameInstance = nullptr;

		
};
