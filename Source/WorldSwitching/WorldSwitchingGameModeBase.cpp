// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameModeBase.h"
#include "PWorldActor.h"
#include "SWorldActor.h"
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
	PlayerController = GetWorld()->GetFirstPlayerController();
	
	
	

	if (CameraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got a CameraComponent"))
	}

	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got first PlayerController"))

			PlayerController->InputComponent->BindAction("ChangeWorlds", IE_Pressed, this, &AWorldSwitchingGameModeBase::ChangeWorlds);
	}

	
	ToggleSpiritCharacters();
	ToggleSWorldActors();
}


void AWorldSwitchingGameModeBase::ChangeWorlds()
{
	WorldTransitionEffects();
	bIsSpiritWorld = !bIsSpiritWorld;
	
	if (bIsSpiritWorld)
	{
		if (CameraComponent)
		{
			CameraComponent->PostProcessSettings.VignetteIntensity = 1.0f;
		}

		TogglePWorldActors();
		ToggleSWorldActors();
		ToggleSpiritCharacters();
		ToggleParticleEffects();
	}

	else
	{
		if (CameraComponent)
		{
			CameraComponent->PostProcessSettings.VignetteIntensity = 0.0f;
		}
		ToggleSpiritCharacters();
		TogglePWorldActors();
		ToggleSWorldActors();
		ToggleParticleEffects();
	}
}

void AWorldSwitchingGameModeBase::TogglePWorldActors()
{
	if (bIsSpiritWorld)
	{
		//Alle PWorldActor settes som usynlig uten collision
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{
			APWorldActor *PWorldActor = *PActorItr;

			PActorItr->SetActorEnableCollision(false);
			PActorItr->SetActorHiddenInGame(true);
			
		}
	}

	else
	{
		//Alle PWorldActor settes som synlig m/collision
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{
			APWorldActor *PWorldActor = *PActorItr;

			PActorItr->SetActorEnableCollision(true);
			PActorItr->SetActorHiddenInGame(false);
		}
	}
}

void AWorldSwitchingGameModeBase::ToggleSWorldActors()
{
	if (bIsSpiritWorld)
	{
		//Alle PWorldActor settes som usynlig uten collision
		for (TActorIterator<ASWorldActor> SActorItr(GetWorld()); SActorItr; ++SActorItr)
		{
			ASWorldActor *SWorldActor = *SActorItr;

			SActorItr->SetActorEnableCollision(true);
			SActorItr->SetActorHiddenInGame(false);

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
	}
}

void AWorldSwitchingGameModeBase::ToggleSpiritCharacters()
{
	if (bIsSpiritWorld)
	{
		//Gjøre EnemySpirits synlig m/collision
		for (TActorIterator<ASpiritTest> SpiritItr(GetWorld()); SpiritItr; ++SpiritItr)
		{
			ASpiritTest *SpiritTest = *SpiritItr;
			SpiritItr->SetActorEnableCollision(true);
			SpiritItr->SetActorHiddenInGame(false);
		}
	}
	else
	{
		//Spirit enemy settes som usynlig u/collision
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
			UE_LOG(LogTemp, Warning, TEXT("Found Spirit!"))

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

