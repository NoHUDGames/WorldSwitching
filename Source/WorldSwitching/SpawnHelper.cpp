// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnHelper.h"

// Sets default values
ASpawnHelper::ASpawnHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyMesh"));

	RootComponent = SceneComponent;
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASpawnHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

