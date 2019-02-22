// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWorldActor.h"
#include "S_PickupShield.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "BP_Character.h"
#include "S_SpiralPopUp.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API AS_SpiralPopUp : public ASWorldActor
{
	GENERATED_BODY()

public:
		AS_SpiralPopUp();

		virtual void BeginPlay() override;

		bool bIsEffectRunning;

		UPROPERTY(EditAnywhere)
		USceneComponent* SceneRoot = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* ShieldSpawnLocation = nullptr;

		UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh = nullptr;

		UPROPERTY(EditAnywhere)
		UCapsuleComponent* WalkInTrigger = nullptr;

		UPROPERTY(EditAnywhere)
		UParticleSystemComponent* GroundSmokeEffect = nullptr;

		ABP_Character* OurPawn = nullptr;

		FVector DummySpawnOffset;

		UFUNCTION()
		void PlayerOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION(BlueprintImplementableEvent)
			void DriveDummyThroughFloor();

		UFUNCTION(BlueprintCallable)
			ASWorldActor* GetDummySpiral();

		FTimerHandle EndSequenceHandle;
		FTimerHandle DriveDummyHandle;

		void TurnOffSequence();


		UPROPERTY(EditAnywhere, Category = Spawning)
		//Need Complex collision. Must be static, cant be moveable
		TSubclassOf<ASWorldActor> DummySpiralToSpawn;

		UPROPERTY(EditAnywhere, Category = Spawning)
		TSubclassOf<AS_PickupShield> ShieldToSpawn;

private:
		ASWorldActor* DummySpiralReference = nullptr;
};
