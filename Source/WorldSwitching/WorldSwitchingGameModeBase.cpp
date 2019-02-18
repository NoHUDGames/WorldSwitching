// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameModeBase.h"
#include "PWorldActor.h"
#include "SWorldActor.h"
#include "PSWorldActor.h"
#include "SpiritTest.h"
#include "ParticleEffectActor.h"
#include "Kismet/GameplayStatics.h"

//class ASpiritTest;
//class APWorldActor;

AWorldSwitchingGameModeBase::AWorldSwitchingGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsSpiritWorld = false;
}

void AWorldSwitchingGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWorldSwitchingGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	EAutoReceiveInput::Player0;
	CameraComponent = Cast<UCameraComponent>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass(UCameraComponent::StaticClass()));
	PlayerCapsuleCollision = Cast<UCapsuleComponent>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass(UCapsuleComponent::StaticClass()));
	PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerPawn = Cast<APawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got Player Pawn"))
	}
	
	
	if (PlayerCapsuleCollision)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got a Player Capsule Collision"))
	}
	
	if (CameraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got a CameraComponent"))
	}

	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got first PlayerController"))

			PlayerController->InputComponent->BindAction("ChangeWorlds", IE_Pressed, this, &AWorldSwitchingGameModeBase::ChangeWorlds);
	}

	ToggleSpiritWorldActors();
}


void AWorldSwitchingGameModeBase::ChangeWorlds()
{
	//AActor* OverlappingActor = PlayerCapsuleCollision->Overlap
	//PlayerCapsuleCollision->GetOverlapsWithActor()

	WorldTransitionEffects();
	bIsSpiritWorld = !bIsSpiritWorld;

	TogglePhysicalWorldActors();
	ToggleSpiritWorldActors();
	ToggleParticleEffects();
	ToggleLastingCameraEffects();
	TogglePhysicalSpiritMaterialProperties();
}

void AWorldSwitchingGameModeBase::TogglePhysicalWorldActors()
{
		//Alle PWorldActor settes som usynlig uten collision
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{

			APWorldActor *PWorldActor = *PActorItr;

			if (bIsSpiritWorld)
			{
				PActorItr->SetActorEnableCollision(false);
				PActorItr->SetActorHiddenInGame(true);
			}
			else
			{
				PActorItr->SetActorEnableCollision(true);
				PActorItr->SetActorHiddenInGame(false);
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

			SActorItr->SetActorEnableCollision(true);
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
		//Alle PWorldActor settes som usynlig uten collision
		for (TActorIterator<ASWorldActor> SActorItr(GetWorld()); SActorItr; ++SActorItr)
		{
			ASWorldActor *SWorldActor = *SActorItr;

			SActorItr->SetActorEnableCollision(false);
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
			AParticleEffectActor *SpiritTest = *ParticleItr;

			ParticleItr->PhysicalWorldParticles->Deactivate();
			ParticleItr->SpiritWorldParticles->Activate();
		}
	}
	else
	{
		for (TActorIterator<AParticleEffectActor> ParticleItr(GetWorld()); ParticleItr; ++ParticleItr)
		{
			AParticleEffectActor *SpiritTest = *ParticleItr;
			
			ParticleItr->PhysicalWorldParticles->Activate();
			ParticleItr->SpiritWorldParticles->Deactivate();
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
	UE_LOG(LogTemp,Warning, TEXT("InsideTogglePhysicalSpiritMaterialProperties"))

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