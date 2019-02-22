// Fill out your copyright notice in the Description page of Project Settings.

#include "S_SpiralPopUp.h"

AS_SpiralPopUp::AS_SpiralPopUp()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	WalkInTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Walk in trigger"));
	ShieldSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ShieldSpawnLocatin"));
	GroundSmokeEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GroundSmoke"));


	RootComponent = SceneRoot;
	Mesh->SetupAttachment(RootComponent);
	WalkInTrigger->SetupAttachment(RootComponent);
	ShieldSpawnLocation->SetupAttachment(RootComponent);
	GroundSmokeEffect->SetupAttachment(RootComponent);

	DummySpawnOffset = { 0.f, 0.f, -800.f };
	bIsEffectRunning = false;
}

void AS_SpiralPopUp::BeginPlay()
{
	Super::BeginPlay();
	Mesh->ToggleVisibility(false);
	

	FVector SpawnLocation = GetActorLocation() + DummySpawnOffset;

	WalkInTrigger->OnComponentBeginOverlap.AddDynamic(this, &AS_SpiralPopUp::PlayerOverlaps);

	DummySpiralReference = GetWorld()->SpawnActor<ASWorldActor>(DummySpiralToSpawn, SpawnLocation, FRotator(0.f, 0.f, 0.f));
}

void AS_SpiralPopUp::PlayerOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		if (OtherActor->IsA(ABP_Character::StaticClass()) && !bIsEffectRunning)
		{
			WalkInTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			OurPawn = Cast<ABP_Character>(OtherActor);
			
			if (OurPawn)
			{
				OurPawn->DisableInput(Cast<APlayerController>(OurPawn->GetController()));
				OurPawn->SetActorTickEnabled(false);
			}

			bIsEffectRunning = true;

			GroundSmokeEffect->Activate();

			GetWorldTimerManager().SetTimer(DriveDummyHandle, this, &AS_SpiralPopUp::DriveDummyThroughFloor, 2.f, false);
			GetWorldTimerManager().SetTimer(EndSequenceHandle, this, &AS_SpiralPopUp::TurnOffSequence, 5.f, false);
			
		}
}

void AS_SpiralPopUp::TurnOffSequence()
{
	//GroundSmokeEffect->Deactivate();
	if (OurPawn)
	{
		OurPawn->EnableInput(Cast<APlayerController>(OurPawn->GetController()));
		OurPawn->SetActorTickEnabled(true);
	}

	Mesh->ToggleVisibility(true);

	bIsEffectRunning = false;
}

ASWorldActor* AS_SpiralPopUp::GetDummySpiral()
{
	return DummySpiralReference;
}