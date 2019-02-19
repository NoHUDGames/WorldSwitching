// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWorldActor.h"
#include "Artifacts.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API AArtifacts : public APWorldActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArtifacts();

	UFUNCTION(BlueprintImplementableEvent)
		void PickupFeedback();
	
	virtual void Tick(float DeltaTime) override;
};
