// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWorldActor.h"
#include "SWorldActor.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "SensingSphere.generated.h"

UCLASS()
class WORLDSWITCHING_API ASensingSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASensingSphere();

	FVector Scale = { 1.f, 1.f, 1.f };
	float SpeedScale = 35;

	FTimerHandle KillTimer;

	UFUNCTION()
		void OverlapsWithActors(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void Activate();

	void Kill();

	void TurnOnOtherActorCollisions();

	void TurnOffOtherActorCollisions();

	void AdjustSphereColliderForWorldType();

	class AWorldSwitchingGameModeBase* GameMode = nullptr;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollider = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
