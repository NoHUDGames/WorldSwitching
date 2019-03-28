// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWorldActor.h"
#include "OurEnums.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BelongsToLevel)
		EArtifactBelongsToLevel BelongsToLevel;

	//Determines if the index is sent back to GameInstance for registering as picked up
	UPROPERTY(EditAnywhere)
	bool bKeepTrackOf = true;

	virtual void Tick(float DeltaTime) override;

	void SetArrayIndex(int index);
	int GetArrayIndex();
	
private:
	
	//Used by GameInstance to keep track of artifacts in levels
	int ArrayIndex;
};
