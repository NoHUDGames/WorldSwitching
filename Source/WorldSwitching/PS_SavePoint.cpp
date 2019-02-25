// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_SavePoint.h"
#include "BP_Character.h"

APS_SavePoint::APS_SavePoint()
{
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
}

void APS_SavePoint::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &APS_SavePoint::SetPlayerRespawnLocation);
}

void APS_SavePoint::SetPlayerRespawnLocation(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("INSIDE SAVE POINT OVERLAP"))

	if (OtherActor->IsA(ABP_Character::StaticClass()))
	{
		ABP_Character* Player = Cast<ABP_Character>(OtherActor);
		Player->SetRespawnLocation(GetActorLocation());

		UE_LOG(LogTemp, Warning, TEXT("New save Location is %s"), *Player->GetRespawnLocation().ToString())
	}
}