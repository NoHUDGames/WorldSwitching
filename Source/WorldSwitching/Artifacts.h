// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWorldActor.h"
#include "OurEnums.h"
#include "SpawnHelper.h"
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

	UFUNCTION(BlueprintImplementableEvent)
		void ArtifactLightupWhenSensed();

	//Determines if the index is sent back to GameInstance for registering as picked up
	bool bKeepTrackOf = false;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void SetArrayIndex(int index);
	int GetArrayIndex();

	ASpawnHelper* SpawnedBy = nullptr;
	
private:
	
	//Used by GameInstance to keep track of artifacts in levels
	int ArrayIndex;
};
