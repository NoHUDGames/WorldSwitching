// Fill out your copyright notice in the Description page of Project Settings.

#include "S_SpiralPopUp.h"

AS_SpiralPopUp::AS_SpiralPopUp()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	WalkInTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Walk in trigger"));
	ShieldSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ShieldSpawnLocatin"));
	DummySpiralSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("DummySpiralSpawnLocatin"));


	RootComponent = SceneRoot;
	Mesh->SetupAttachment(RootComponent);
	WalkInTrigger->SetupAttachment(RootComponent);
	ShieldSpawnLocation->SetupAttachment(RootComponent);
	DummySpiralSpawnLocation->SetupAttachment(RootComponent);

	DummySpawnOffset = { 0.f, 0.f, -800.f };
}

void AS_SpiralPopUp::BeginPlay()
{
	Super::BeginPlay();

	Mesh->bHiddenInGame = true;
	

	FVector SpawnLocation = GetActorLocation() + DummySpawnOffset;
	
	WalkInTrigger->OnComponentBeginOverlap.AddDynamic(this, &AS_SpiralPopUp::PlayerOverlaps);

	GetWorld()->SpawnActor<ASWorldActor>(DummySpiralToSpawn, SpawnLocation, FRotator(0.f, 0.f, 0.f));
	
}

void AS_SpiralPopUp::PlayerOverlaps(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OVERLAPPED WITH SPIRAL COLLIDER"))

}
