// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameModeBase.h"
#include "PWorldActor.h"
#include "SWorldActor.h"
#include "PSWorldActor.h"
#include "SpiritTest.h"
#include "PWorldCharacter.h"
#include "ParticleEffectActor.h"
#include "Artifacts.h"
#include "Kismet/GameplayStatics.h"
#include "WorldSwitchingGameInstance.h"

//class ASpiritTest;
//class APWorldActor;

AWorldSwitchingGameModeBase::AWorldSwitchingGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsSpiritWorld = true;
}

void AWorldSwitchingGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWorldSwitchingGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	EAutoReceiveInput::Player0;

	GameInstance = Cast<UWorldSwitchingGameInstance>(GetWorld()->GetGameInstance());
	GameInstance->ManageLevelPickups();
	
	

	CameraComponent = Cast<UCameraComponent>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass(UCameraComponent::StaticClass()));
	PlayerCapsuleCollision = Cast<UCapsuleComponent>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass(UCapsuleComponent::StaticClass()));
	PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerPawn = Cast<ABP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE: Got Player Pawn"))
	}

	PlayerPawn->SetLives(GameInstance->FeedPlayerHealth());
	PlayerPawn->SetArtifacts(GameInstance->FeedPlayerArtifacts());

	if (PlayerCapsuleCollision)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE: Got a Player Capsule Collision"))
	}
	
	if (CameraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE: Got a CameraComponent"))
	}

	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE: Got first PlayerController"))

			PlayerController->InputComponent->BindAction("ChangeWorlds", IE_Pressed, this, &AWorldSwitchingGameModeBase::ChangeWorldsProxy);
	}
	

	if (GameInstance->GetbIsFirstTimeStartingGame())
	{
		GameInstance->SetbIsFirstTimeStartingGame(false);
		ChangeWorlds(false);
	}
	else
	{
		TogglePhysicalWorldActors();
		ToggleParticleEffects();
		TogglePhysicalSpiritMaterialProperties();
		ToggleLastingCameraEffects();
	}
}

void AWorldSwitchingGameModeBase::ChangeWorldsProxy()
{
	ChangeWorlds();
}

void AWorldSwitchingGameModeBase::ChangeWorlds(bool bShowTransitionEffects)
{
	bIsSpiritWorld = !bIsSpiritWorld;
	
	
	if (!bIsSpiritWorld && TestPhysicalOverlaps()) return;

	else if (bIsSpiritWorld)
	{
		UE_LOG(LogTemp,Warning,TEXT("Inside IF for TestSpiritOverlaps"))
		TestSpiritOverlaps();
	}
	
	PlayerPawn->bIsSpiritWorld = bIsSpiritWorld;

	if (bShowTransitionEffects) WorldTransitionEffects();
	
	TogglePhysicalWorldActors();
	ToggleSpiritWorldActors();
	ToggleParticleEffects();
	ToggleLastingCameraEffects();
	TogglePhysicalSpiritMaterialProperties();
}

bool AWorldSwitchingGameModeBase::TestPhysicalOverlaps()
{
		//Momentarily test for overlap with Actors

		PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
		TogglePhysicalWorldActors();

		OtherActorPhysicalTest = PlayerPawn->GetOtherActorForPhysicalTest();

		if (OtherActorPhysicalTest)
		{
			if (OtherActorPhysicalTest->IsA(APWorldActor::StaticClass()) && !OtherActorPhysicalTest->IsA(AArtifacts::StaticClass()))
			{
				/// UE_LOG(LogTemp, Warning, TEXT("Overlapped with PhysicalActor %s"), *OtherActorPhysicalTest->GetActorLabel())
				UGameplayStatics::PlaySound2D(GetWorld(), CannotReturnToPhysical);
				DeniedPhysicalReentryEffects();

				//Stuck in Spirit World
				bIsSpiritWorld = true;
				TogglePhysicalWorldActors();
				LightUpCollidingActor();
				PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
				
				return true;
			}
	}
	UE_LOG(LogTemp, Warning, TEXT("Did NOT Overlap with PhysicalActor"));
	PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	return false;
}

void AWorldSwitchingGameModeBase::TestSpiritOverlaps()
{
	PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	ToggleSpiritWorldActors();
	OtherActorPhysicalTest = PlayerPawn->GetOtherActorForPhysicalTest();

	if (!OtherActorPhysicalTest)
	{
		//Will be set to block again OnComponentEndOverlap from blueprint 
		PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	}
}

AActor* AWorldSwitchingGameModeBase::GetOtherActorPhysicalTest()
{
	return OtherActorPhysicalTest;
}

void AWorldSwitchingGameModeBase::TogglePhysicalWorldActors()
{
	if (bIsSpiritWorld)
	{
		//Alle PWorldActor settes som usynlig uten collision
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{

			APWorldActor *PWorldActor = *PActorItr;

			if (!PActorItr->bOptOutOfCollisionChange)
			PActorItr->SetActorEnableCollision(false);

			if (!PActorItr->bOptOutOfVisibilityChange)
			PActorItr->SetActorHiddenInGame(true);
		}

		// Samme for PWorldCharacters

		for (TActorIterator<APWorldCharacter> PCharItr(GetWorld()); PCharItr; ++PCharItr)
		{
			APWorldCharacter* PWorldChar = *PCharItr;
			PCharItr->SetActorEnableCollision(false);
			PCharItr->SetActorHiddenInGame(true);
		}
	}
	else
	{
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{

			APWorldActor *PWorldActor = *PActorItr;

			if (!PActorItr->bOptOutOfCollisionChange)
			PActorItr->SetActorEnableCollision(true);

			if (!PActorItr->bOptOutOfVisibilityChange)
			PActorItr->SetActorHiddenInGame(false);
		}

		for (TActorIterator<APWorldCharacter> PCharItr(GetWorld()); PCharItr; ++PCharItr)
		{
			APWorldCharacter* PWorldChar = *PCharItr;
			PCharItr->SetActorEnableCollision(true);
			PCharItr->SetActorHiddenInGame(false);
		}
	}
}

void AWorldSwitchingGameModeBase::ToggleSpiritWorldActors()
{
	if (bIsSpiritWorld)
	{
		//SWorldActors synlig m/collision
		for (TActorIterator<ASWorldActor> SActorItr(GetWorld()); SActorItr; ++SActorItr)
		{
			ASWorldActor *SWorldActor = *SActorItr;

			
				if (!SActorItr->bOptOutOfCollisionChange)
					SActorItr->SetActorEnableCollision(true);

				if (!SActorItr->bOptOutOfVisibilityChange)
					SActorItr->SetActorHiddenInGame(false);
			

		}

		//EnemySpirits synlig m/collision
		for (TActorIterator<ASpiritTest> SpiritItr(GetWorld()); SpiritItr; ++SpiritItr)
		{
			ASpiritTest *SpiritTest = *SpiritItr;
			SpiritItr->SetActorEnableCollision(true);
			SpiritItr->SetActorHiddenInGame(false);
		}

	}

	else
	{
		//Alle SWorldActor settes som usynlig uten collision
		for (TActorIterator<ASWorldActor> SActorItr(GetWorld()); SActorItr; ++SActorItr)
		{
			ASWorldActor *SWorldActor = *SActorItr;
			
			
				if (!SActorItr->bOptOutOfCollisionChange)
					SActorItr->SetActorEnableCollision(false);

				if (!SActorItr->bOptOutOfVisibilityChange)
					SActorItr->SetActorHiddenInGame(true);
			
		}

		//EnemySpirit settes som usynlig u/collision
		for (TActorIterator<ASpiritTest> SpiritItr(GetWorld()); SpiritItr; ++SpiritItr)
		{
			ASpiritTest *SpiritTest = *SpiritItr;
			SpiritItr->SetActorEnableCollision(false);
			SpiritItr->SetActorHiddenInGame(true);
		}
	}
}


void AWorldSwitchingGameModeBase::ToggleParticleEffects()
{
	if (bIsSpiritWorld)
	{
		for (TActorIterator<AParticleEffectActor> ParticleItr(GetWorld()); ParticleItr; ++ParticleItr)
		{
			AParticleEffectActor *Particle = *ParticleItr;

			if (ParticleItr->bIsDynamicFire)
			{
				ParticleItr->DynamicBothWorlds->SetVectorParameter("Color", ParticleItr->SpiritFireColor);
			}

			else
			{
				if (!ParticleItr->bOptOutOfWorldChangeEffect)
				{
					ParticleItr->PhysicalWorldParticles->Deactivate();
					ParticleItr->SpiritWorldParticles->Activate();
				}
			}
		}
	}
	else
	{
		for (TActorIterator<AParticleEffectActor> ParticleItr(GetWorld()); ParticleItr; ++ParticleItr)
		{
			AParticleEffectActor *Particle = *ParticleItr;
			
			if (ParticleItr->bIsDynamicFire)
			{
				ParticleItr->DynamicBothWorlds->SetVectorParameter("Color", ParticleItr->PhysicalFireColor);
			}

			else
			{
				if (!ParticleItr->bOptOutOfWorldChangeEffect)
				{
					ParticleItr->PhysicalWorldParticles->Activate();
					ParticleItr->SpiritWorldParticles->Deactivate();
				}
			}
		}
	}
}

void AWorldSwitchingGameModeBase::ToggleLastingCameraEffects()
{
	if (bIsSpiritWorld && CameraComponent)
	{
		CameraComponent->PostProcessSettings.VignetteIntensity = 1.0f;
	}

	else if (!bIsSpiritWorld && CameraComponent)
	{
		CameraComponent->PostProcessSettings.VignetteIntensity = 0.0f;
	}

}

void AWorldSwitchingGameModeBase::TogglePhysicalSpiritMaterialProperties()
{


	for (TActorIterator<APSWorldActor> PSActorItr(GetWorld()); PSActorItr; ++PSActorItr)
	{
		APSWorldActor *PSActor = *PSActorItr;

		if (!PSActor->bCanChangeMaterial) continue;

		else if (bIsSpiritWorld)
		{
			PSActor->ActivateSpiritMaterialProperties();
		}

		else if (!bIsSpiritWorld)
		{
			PSActor->ActivatePhysicalMaterialProperties();
		}
	}
}