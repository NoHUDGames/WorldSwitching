// Fill out your copyright notice in the Description page of Project Settings.

#include "SWorldActor.h"

// Sets default values
ASWorldActor::ASWorldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bOptOutOfCollisionChange = false;
}

// Called when the game starts or when spawned
void ASWorldActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

