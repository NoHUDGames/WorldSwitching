// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSWorldActor.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "WorldSwitchingGameInstance.h"
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

	UFUNCTION(BlueprintCallable)
		void SetGoddessMeshComponent(USkeletalMeshComponent* ComponentToGet) {
		GoddessMesh = ComponentToGet;
		
		if(GoddessMesh) UE_LOG(LogTemp, Warning, TEXT("Got GoddessMeshComponent"))

};

	UWorldSwitchingGameInstance* GameInstance = nullptr;

	void ReceivingArtifacts(int PlayerHoldingArtifacts);

	void OpenNewPortals();

	UPROPERTY(BlueprintReadWrite)
	int DroppedOffArtifacts = 0;
};
