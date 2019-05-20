// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"
#include "WorldSwitchingGameInstance.h"
#include "Camera/CameraActor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Altar.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API AAltar : public APSWorldActor
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AAltar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		USphereComponent* ArtifactDropoffCollider {
		nullptr
	};

	USkeletalMeshComponent* GoddessMesh = nullptr;

	UPROPERTY(EditAnywhere)
	USceneComponent* Level1PortalLookatStart = nullptr;

	UPROPERTY(EditAnywhere)
	USceneComponent* Level2PortalLookatStart = nullptr;

	FVector ActivationSequenceCameraPoint;

	UFUNCTION(BlueprintCallable)
		void SetGoddessMeshComponent(USkeletalMeshComponent* ComponentToGet) {
		GoddessMesh = ComponentToGet;
		
		if(GoddessMesh) UE_LOG(LogTemp, Warning, TEXT("Got GoddessMeshComponent"))

};

	//Start | Toggle appearance of goddess between worlds
	UPROPERTY(EditAnywhere, Category = Goddess)
	UMaterialInterface* SpiritMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = Goddess)
	UMaterialInterface* PhysicalMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = Goddess)
		UAnimSequence* SpiritAnimation = nullptr;

	UPROPERTY(EditAnywhere, Category = Goddess)
		UAnimSequence* PhysicalAnimation = nullptr;

	void ActivatePhysicalGoddess();
	void ActivateSpiritGoddess();

	UWorldSwitchingGameInstance* GameInstance = nullptr;
	//End | Toggle appearance of goddess between worlds


	//Start | Sequence for opening new portals after artifact delivery
	UPROPERTY(BlueprintReadWrite)
	ACameraActor* m_FreeLevelCamera;

	class APS_Portal* PortalToOpen = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* ActivationBeam = nullptr;

	void ReceivingArtifacts(int PlayerHoldingArtifacts);

	UFUNCTION(BlueprintCallable)
	void SetFreeLevelCamera(ACameraActor* FreeLevelCamera) { m_FreeLevelCamera = FreeLevelCamera; }

	void OpenNewPortals();
	
	void PortalActivationSequence();

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* ActivationSequenceSound = nullptr;


	UFUNCTION(BlueprintImplementableEvent)
	void SpawnBeam(FVector Target);

	UFUNCTION(BlueprintImplementableEvent)
	void KillBeam();

	
	FTimerHandle ActivateBeamHandle;
	FTimerHandle ActivatePortalHandle;
	FTimerHandle EndSequenceHandle;

	void ActivateBeam();
	void ActivatePortal();
	void EndSequence();

	UFUNCTION(BlueprintImplementableEvent)
	void LerpCamera(ACameraActor* FreeCamera, class APS_Portal* Portal, FVector Start, FVector End);
	//End | Sequence for opening new portals after artifact delivery

	UPROPERTY(BlueprintReadWrite)
	int DroppedOffArtifacts = 0;
};
