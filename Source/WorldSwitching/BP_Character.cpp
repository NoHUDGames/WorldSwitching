// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_Character.h"

// Sets default values
ABP_Character::ABP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABP_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveUp", this, &ABP_Character::MoveUp);
	PlayerInputComponent->BindAxis("MoveDown", this, &ABP_Character::MoveDown);
	PlayerInputComponent->BindAxis("MoveLeft", this, &ABP_Character::MoveLeft);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABP_Character::MoveRight);

}

void ABP_Character::MoveUp(float AxisValue)
{//50.f, 0.f, 0.f
	AddMovementInput(FVector(50.f, 0.f, 0.f), AxisValue);
}

void ABP_Character::MoveDown(float AxisValue)
{//-50.f, 0.f, 0.f
	AddMovementInput(FVector(-50.f, 0.f, 0.f), AxisValue);
}
void ABP_Character::MoveLeft(float AxisValue)
{//0.f, -50.f, 0.f
	AddMovementInput(FVector(0.f, -50.f, 0.f), AxisValue);
}
void ABP_Character::MoveRight(float AxisValue)
{//0.f, 50.f, 0.f
	AddMovementInput(FVector(0.f, 50.f, 0.f), AxisValue);
}