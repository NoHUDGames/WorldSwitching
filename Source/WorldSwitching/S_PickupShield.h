// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWorldActor.h"
#include "OurEnums.h"
#include "Components/SphereComponent.h"
#include "S_PickupShield.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API AS_PickupShield : public ASWorldActor
{
	GENERATED_BODY()

		virtual void Tick(float DeltaTime) override;
public:

	//START | Lighting up when sensing

	//Because we added meshcomponent and spherecomponent from blueprint and use them in other contexts
	//its more painless to just get a reference this way instead of adding from C++ now and deal with other things
	//breaking.
	UStaticMeshComponent* ShieldMesh = nullptr;
	USphereComponent* SphereCollider = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ShieldOriginalMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ShieldLightUpMaterial = nullptr;

	UMaterialInstanceDynamic* ShieldDynamicMaterial = nullptr;


	UFUNCTION(BlueprintCallable)
		void SetShieldMeshReference(UStaticMeshComponent* ArtifactMeshToGet) {
		ShieldMesh = ArtifactMeshToGet;
	}

	UFUNCTION(BlueprintCallable)
		void SetSphereColliderReference(USphereComponent* SphereToGet) { SphereCollider = SphereToGet; }


	UFUNCTION(BlueprintImplementableEvent)
		void TriggerShieldLightUpTimeline();

	UFUNCTION(BlueprintCallable)
		void LightUpShield(float TimeLine);


	virtual void LightUpActorWhenSensed() override;

	UFUNCTION(BlueprintCallable)
		void ResetAfterLightUp();

	//Used to check weather player has changed world while artifact is lighting up from sensing, in that case it will finish straight away,
	//and reapply original material. See blueprint implementation of TriggerArtifactLightUpTimeLine
	UPROPERTY(BlueprintReadOnly)
		bool m_bIsSpiritWorld = false;

	//END | Lighting up when sensing
	UFUNCTION(BlueprintImplementableEvent)
		void PickupFeedback();

	UPROPERTY(EditAnywhere)
	bool bKeepTrackOf = true;

	//called in GameInstance after spawn
	void SetArrayIndex(int index);
	int GetArrayIndex();
	


private:
	int ArrayIndex = 0;
};
