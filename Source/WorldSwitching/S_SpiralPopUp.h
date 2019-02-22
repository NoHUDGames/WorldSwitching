// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWorldActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "S_SpiralPopUp.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API AS_SpiralPopUp : public ASWorldActor
{
	GENERATED_BODY()


		AS_SpiralPopUp();

		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere)
		USceneComponent* SceneRoot = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* ShieldSpawnLocation = nullptr;

		UPROPERTY(EditAnywhere)
		USceneComponent* DummySpiralSpawnLocation = nullptr;

		UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh = nullptr;

		UPROPERTY(EditAnywhere)
		UCapsuleComponent* WalkInTrigger = nullptr;

		FVector DummySpawnOffset;

		UFUNCTION()
		void PlayerOverlaps(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);




		UPROPERTY(EditAnywhere, Category = DummyMoveableSpiral)
		//Need Complex collision. Must be static, cant be moveable
		TSubclassOf<ASWorldActor> DummySpiralToSpawn;


	
};
