// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWorldActor.h"
#include "SWorldActor.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SensingSphere.generated.h"

UCLASS()
class WORLDSWITCHING_API ASensingSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASensingSphere();

	UPROPERTY(EditAnywhere)
		USphereComponent* SphereCollider = nullptr;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BallMesh = nullptr;

	UPROPERTY(EditAnywhere)
		class ABP_Character* PlayerPawn = nullptr;

	FVector Scale = { 1.f, 1.f, 1.f };
	float SpeedScale = 240;

	FTimerHandle KillTimer;

	UFUNCTION()
		void OverlapsWithActors(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//Starts to expand and overlap with objects
	void Activate();

	void Kill();

	//Because objects are hidden, this temporarily turns on their collision so they can be sensed
	void TurnOnOtherActorCollisions();

	//Needed different treatments because pickups only overlap, not block. Enables player to sense pickup
	//while standing inside bounds of pickup without picking it up
	void PrepareArtifactForSensing(AActor* ArtifactActor);
	void PrepareShieldForSensing(AActor* ArtifactActor);

	//Resets objects after sensing
	void TurnOffOtherActorCollisions();

	//Sets its own collision to overlap with correct type according to world
	void AdjustSphereColliderForWorldType();

	class AWorldSwitchingGameModeBase* GameMode = nullptr;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
