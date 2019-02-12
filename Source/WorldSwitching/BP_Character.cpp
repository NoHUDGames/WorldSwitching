// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_Character.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ABP_Character::ABP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root component for kicking
	KickingRotation = CreateDefaultSubobject<USceneComponent>(TEXT("KickingRotation"));
	KickingRotation->SetupAttachment(RootComponent);
	KickingRotation->Mobility = EComponentMobility::Movable;
	KickingRotation->bVisualizeComponent = true;

	// Create and position a mesh component so we can see where our sphere is 
	// DUMMY mesh, only for prototype. Will exchange when we have animation for kicking
	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FakeLeg"));
	SphereVisual->SetupAttachment(KickingRotation);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->bVisualizeComponent = true;
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
		SphereVisual->Mobility = EComponentMobility::Movable;
		SphereVisual->SetWorldScale3D(FVector(0.2f));
	}

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("FootCollider"));
	BoxCollider->SetupAttachment(SphereVisual);

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
	PlayerInputComponent->BindAxis("MoveRight", this, &ABP_Character::MoveRight);
	PlayerInputComponent->BindAction("Kicking", IE_Pressed, this, &ABP_Character::Kicking);

}

void ABP_Character::MoveUp(float AxisValue)
{//50.f, 0.f, 0.f
	AddMovementInput(FVector(50.f, 0.f, 0.f), AxisValue);
}

void ABP_Character::MoveRight(float AxisValue)
{//0.f, 50.f, 0.f
	AddMovementInput(FVector(0.f, 50.f, 0.f), AxisValue);
}

void ABP_Character::Kicking()
{
	if (!CurrentlyKicking)
	{
		/// Makes it impossible for the player to kick while it's already kicking
		CurrentlyKicking = true;

		/// Turns on overlapping with other pawns for the kick box collider
		BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		
		/// Rotates the scene component, so the kick kan hit something
		FRotator NewRotation{ 90.f, 0.f ,0.f };
		KickingRotation->AddLocalRotation(NewRotation);

		/// Resets the kick after 0.3 seconds
		GetWorldTimerManager().SetTimer(Timer, this, &ABP_Character::StopKicking, 0.3f, false);
		
	}
	
}

void ABP_Character::StopKicking()
{
	/// Resets all values set in the function Kicking
	FRotator NewRotation{ -90.f,0.f ,0.f };
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	KickingRotation->AddLocalRotation(NewRotation);
	CurrentlyKicking = false;
}




