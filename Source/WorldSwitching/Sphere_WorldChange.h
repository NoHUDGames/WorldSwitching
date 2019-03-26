// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Sphere_WorldChange.generated.h"

UCLASS()
class WORLDSWITCHING_API ASphere_WorldChange : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASphere_WorldChange();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BallMesh = nullptr;

	FVector Scale = { 1.f, 1.f, 1.f };
	float SpeedScale = 200;

	FTimerHandle KillTimer;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
