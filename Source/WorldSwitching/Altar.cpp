// Fill out your copyright notice in the Description page of Project Settings.

#include "Altar.h"
#include "BP_Character.h"
#include "PS_Portal.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AAltar::AAltar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/// Makes a collider that will be used to test if the player is in range of dropping off artifacts
	ArtifactDropoffCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ArtifactDropoffCollider"));
	ArtifactDropoffCollider->SetupAttachment(RootComponent);
	ArtifactDropoffCollider->SetSphereRadius(300.f);

	ActivationBeam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ActivationBeamParticle"));
	Level1PortalLookatStart = CreateDefaultSubobject<USceneComponent>(TEXT("Level1 Portal lookat start"));
	Level2PortalLookatStart = CreateDefaultSubobject<USceneComponent>(TEXT("Level2 Portal lookat start"));



	Level1PortalLookatStart->SetupAttachment(RootComponent);
	Level2PortalLookatStart->SetupAttachment(RootComponent);
	ActivationBeam->SetupAttachment(RootComponent);
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
			
			if (Portal->PortalIndex == EPortalIndex::HubLevel_2)
			{
				
				GameInstance->SetHubPortalLevel2Open(true);
				ActivationSequenceCameraPoint = Level2PortalLookatStart->GetComponentLocation();
				UE_LOG(LogTemp, Warning, TEXT("ALTAR: Inside test for Level 2 Portal. LookaAtStart: %s"), *Level2PortalLookatStart->GetComponentLocation().ToString())
				PortalToOpen = Portal;
			}

			PortalActivationSequence();
			
		}
	}
}

void AAltar::PortalActivationSequence()
{

	UGameplayStatics::GetPlayerPawn(GetWorld(),0)->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(),0));
	m_FreeLevelCamera->SetActorLocation(ActivationSequenceCameraPoint);
	m_FreeLevelCamera->SetActorRotation(FRotator(UKismetMathLibrary::FindLookAtRotation(ActivationSequenceCameraPoint, 
		PortalToOpen->GetActorLocation())));

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(m_FreeLevelCamera, 1.f);

	LerpCamera(m_FreeLevelCamera, PortalToOpen, ActivationSequenceCameraPoint + FVector(-300, 0, -400.f), ActivationSequenceCameraPoint + FVector(0, -500, 400));

	UGameplayStatics::PlaySound2D(GetWorld(), ActivationSequenceSound);

	GetWorldTimerManager().SetTimer(ActivateBeamHandle, this, &AAltar::ActivateBeam, 1.5, false);
	GetWorldTimerManager().SetTimer(ActivatePortalHandle, this, &AAltar::ActivatePortal, 2.5f, false);
	GetWorldTimerManager().SetTimer(EndSequenceHandle, this, &AAltar::EndSequence, 6.f, false);


}

void AAltar::ActivateBeam()
{
	SpawnBeam(PortalToOpen->GetActorLocation() + FVector(0,0,150));
	ActivationBeam->Activate();
}

void AAltar::ActivatePortal()
{
	PortalToOpen->Activate(true);
}

void AAltar::EndSequence()
{
	UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(UGameplayStatics::GetPlayerPawn(GetWorld(), 0), 2.f);
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