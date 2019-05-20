// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "Components/BoxComponent.h"
#include "PS_SavePoint.generated.h"

/**
 *  Sets the RespawnLocation variable in the character class, and wether or not worldchange is needed
 * when respawning, in case savepoint is on either spirit or physical-only ground
 */
UCLASS()
class WORLDSWITCHING_API APS_SavePoint : public APSWorldActor
{
	GENERATED_BODY()

public:

		APS_SavePoint();

		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere)
		UBoxComponent* BoxCollision = nullptr;

		//If savepoint is on spirit ground
		UPROPERTY(EditAnywhere)
		EWorld WorldRequiredOnRespawn = EWorld::None;
	
		UFUNCTION()
		void SetPlayerRespawnLocation(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
