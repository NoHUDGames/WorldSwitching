// Fill out your copyright notice in the Description page of Project Settings.

#include "PShamanEnemy.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

APShamanEnemy::APShamanEnemy()
{
	WeaponRotation = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRotation"));
	WeaponRotation->SetupAttachment(RootComponent);
	WeaponRotation->Mobility = EComponentMobility::Movable;
	WeaponRotation->bVisualizeComponent = true;

	WeaponVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempWeapon"));
	WeaponVisual->SetupAttachment(WeaponRotation);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponVisualAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (WeaponVisualAsset.Succeeded())
	{
		WeaponVisual->bVisualizeComponent = true;
		WeaponVisual->SetStaticMesh(WeaponVisualAsset.Object);
		WeaponVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
		WeaponVisual->Mobility = EComponentMobility::Movable;
		WeaponVisual->SetWorldScale3D(FVector(0.2f));
	}

	/// Weapon collider, the collision sphere that damages the player when using the weapon
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	BoxCollider->SetupAttachment(WeaponVisual);
}

void APShamanEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void APShamanEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APShamanEnemy::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APShamanEnemy::DecrementingLives()
{
	if (Lives > 0)
	{
		--Lives;
		UE_LOG(LogTemp, Warning, TEXT("Enemy has %i lives left"), Lives)

	}
}

void APShamanEnemy::KillingEnemy()
{
	if (Lives <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy is dying!"))
			Destroy();
	}
}
