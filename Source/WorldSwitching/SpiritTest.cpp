// Fill out your copyright notice in the Description page of Project Settings.

#include "SpiritTest.h"

// Sets default values
ASpiritTest::ASpiritTest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpiritTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpiritTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpiritTest::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

