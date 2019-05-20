// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWorldActor.h"
#include "OurEnums.h"
#include "SpawnHelper.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
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


	class AWorldSwitchingGameModeBase* GameModeRef = nullptr;

	//START | Lighting up when sensing

	//Because we added meshcomponent and spherecomponent from blueprint and use them in other contexts
	//its more painless to just get a reference this way instead of adding from C++ now and deal with other things
	//breaking.
	UStaticMeshComponent* ArtifactMesh = nullptr;
	USphereComponent* SphereCollider = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ArtifactOriginalMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ArtifactLightUpMaterial = nullptr;

	UMaterialInstanceDynamic* ArtifactDynamicMaterial = nullptr;


	UFUNCTION(BlueprintCallable)
	void SetArtifactMeshReference(UStaticMeshComponent* ArtifactMeshToGet) {	
		ArtifactMesh = ArtifactMeshToGet; }

	UFUNCTION(BlueprintCallable)
	void SetSphereColliderReference(USphereComponent* SphereToGet) { SphereCollider = SphereToGet; }


	UFUNCTION(BlueprintImplementableEvent)
	void PickupFeedback();

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerArtifactLightUpTimeline();

	UFUNCTION(BlueprintCallable)
	void LightUpArtifact(float TimeLine);


	virtual void LightUpActorWhenSensed() override;

	UFUNCTION(BlueprintCallable)
	void ResetAfterLightUp();

	//Used to check weather player has changed world while artifact is lighting up from sensing, in that case it will finish straight away,
	//and reapply original material. See blueprint implementation of TriggerArtifactLightUpTimeLine
	UPROPERTY(BlueprintReadOnly)
	bool m_bIsSpiritWorld = false;

	//END | Lighting up when sensing



	//Determines if the index is sent back to GameInstance for registering as picked up
	bool bKeepTrackOf = false;

	bool bIsLitUpBySensing = false;

	
	virtual void Tick(float DeltaTime) override;

	void SetArrayIndex(int index);
	int GetArrayIndex();

	
private:
	
	//Used by GameInstance to keep track of artifacts in levels
	int ArrayIndex;
};
