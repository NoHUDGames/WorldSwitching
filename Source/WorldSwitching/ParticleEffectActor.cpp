// Fill out your copyright notice in the Description page of Project Settings.

#include "ParticleEffectActor.h"

// Sets default values
AParticleEffectActor::AParticleEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ParticleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ParticleRoot"));
	PhysicalWorldParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PhysicalWorldParticles"));
	SpiritWorldParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpiritWorldParticles"));

	RootComponent = ParticleRoot;

	SpiritWorldParticles->SetupAttachment(RootComponent);
	PhysicalWorldParticles->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AParticleEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AParticleEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

