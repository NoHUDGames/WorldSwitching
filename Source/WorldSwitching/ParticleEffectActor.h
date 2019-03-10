// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ParticleEffectActor.generated.h"

UCLASS()
class WORLDSWITCHING_API AParticleEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticleEffectActor();

	
	USceneComponent* ParticleRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* PhysicalWorldParticles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* SpiritWorldParticles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* DynamicBothWorlds = nullptr;

	UPROPERTY(EditAnywhere, Category = DynamicFire)
		//Set true to run dynamic change to DynamicBothWorlds component
	bool bIsDynamicFire;

	bool bOptOutOfWorldChangeEffect = false;

	FVector PhysicalFireColor;
	FVector SpiritFireColor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
