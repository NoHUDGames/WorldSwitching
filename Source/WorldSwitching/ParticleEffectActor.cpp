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
	DynamicBothWorlds = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DynamicBothWorlds"));

	RootComponent = ParticleRoot;

	SpiritWorldParticles->SetupAttachment(RootComponent);
	PhysicalWorldParticles->SetupAttachment(RootComponent);
	DynamicBothWorlds->SetupAttachment(RootComponent);

	PhysicalFireColor = { 1.f, 0.376471, 0.f };
	SpiritFireColor = { 0.f, 0.536935f, 1.f };

}

// Called when the game starts or when spawned
void AParticleEffectActor::BeginPlay()
{
	Super::BeginPlay();
	SpiritWorldParticles->Deactivate();
}

// Called every frame
void AParticleEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

