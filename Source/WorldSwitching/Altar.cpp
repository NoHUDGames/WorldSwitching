// Fill out your copyright notice in the Description page of Project Settings.

#include "Altar.h"
#include "BP_Character.h"
#include "PS_Portal.h"
#include "EngineUtils.h"

// Sets default values
AAltar::AAltar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/// Makes a collider that will be used to test if the player is in range of dropping off artifacts
	ArtifactDropoffCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ArtifactDropoffCollider"));
	ArtifactDropoffCollider->SetupAttachment(RootComponent);
	ArtifactDropoffCollider->SetSphereRadius(300.f);

}

// Called when the game starts or when spawned
void AAltar::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UWorldSwitchingGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance) DroppedOffArtifacts = GameInstance->GetAltarArtifacts();
}

// Called every frame
void AAltar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAltar::ReceivingArtifacts(int PlayerHoldingArtifacts)
{
	DroppedOffArtifacts += PlayerHoldingArtifacts;
	GameInstance->SetAltarArtifacts(DroppedOffArtifacts);
	
	OpenNewPortals();
}

void AAltar::OpenNewPortals()
{
	for (TActorIterator<APS_Portal> PortalItr(GetWorld()); PortalItr; ++PortalItr)
	{
		APS_Portal *Portal = *PortalItr;
		if (DroppedOffArtifacts >= Portal->GetArtifactsNeededToUse() && !Portal->bIsActive)
		{
			Portal->Activate();

			if (Portal->PortalIndex == EPortalIndex::HubLevel_2)
				GameInstance->SetHubPortalLevel2Open(true);
		}
	}
}

void AAltar::ActivatePhysicalGoddess()
{
	if (GoddessMesh)
	{
		GoddessMesh->SetMaterial(0, PhysicalMaterial);
		GoddessMesh->PlayAnimation(PhysicalAnimation, false);
	}
}

void AAltar::ActivateSpiritGoddess()
{
	if (GoddessMesh)
	{
		GoddessMesh->SetMaterial(0, SpiritMaterial);
		GoddessMesh->PlayAnimation(SpiritAnimation, true);
	}
}