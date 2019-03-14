// Fill out your copyright notice in the Description page of Project Settings.

#include "Altar.h"
#include "BP_Character.h"

// Sets default values
AAltar::AAltar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/// Makes a collider that will be used to test if the player is in range of dropping off artifacts
	ArtifactDropoffCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ArtifactDropoffCollider"));
	ArtifactDropoffCollider->SetupAttachment(RootComponent);
	ArtifactDropoffCollider->SetSphereRadius(300.f);
	ArtifactDropoffCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	DroppedOffArtifacts = 0;
}

// Called when the game starts or when spawned
void AAltar::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAltar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAltar::ReceivingArtifacts(int PlayerHoldingArtifacts)
{
	DroppedOffArtifacts = DroppedOffArtifacts + PlayerHoldingArtifacts;

	UE_LOG(LogTemp, Warning, TEXT("Artifacts dropped off at the altar now: %i"), DroppedOffArtifacts)
}
