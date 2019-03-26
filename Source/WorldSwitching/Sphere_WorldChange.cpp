// Fill out your copyright notice in the Description page of Project Settings.

#include "Sphere_WorldChange.h"

// Sets default values
ASphere_WorldChange::ASphere_WorldChange()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));

	RootComponent = BallMesh;
}

// Called when the game starts or when spawned
void ASphere_WorldChange::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASphere_WorldChange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Scale += FVector(DeltaTime*SpeedScale, DeltaTime*SpeedScale, DeltaTime*SpeedScale);

	SetActorScale3D(Scale);

}

