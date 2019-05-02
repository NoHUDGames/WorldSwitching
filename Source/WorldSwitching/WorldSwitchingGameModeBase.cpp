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
	GatherLevelCameras();

	EAutoReceiveInput::Player0;

	GameInstance = Cast<UWorldSwitchingGameInstance>(GetWorld()->GetGameInstance());
	CameraComponent = Cast<UCameraComponent>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass(UCameraComponent::StaticClass()));
	PlayerCapsuleCollision = Cast<UCapsuleComponent>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass(UCapsuleComponent::StaticClass()));
	PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerPawn = Cast<ABP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	

	if (PlayerPawn)
	{
		PlayerPawn->SetLives(GameInstance->GetPlayerHealth());
		PlayerPawn->SetArtifacts(GameInstance->GetPlayerArtifacts());
		PlayerPawn->SetbIsSpiritWorld(bIsSpiritWorld);
	}

	if (PlayerController)
	{
			PlayerController->InputComponent->BindAction("ChangeWorlds", IE_Pressed, this, &AWorldSwitchingGameModeBase::ChangeWorldsProxy);
	}
	
	if (GameInstance)
	{
		if (GameInstance->GetbIsFirstTimeStartingGame())
		{
			GameInstance->SetbIsFirstTimeStartingGame(false);

			GameInstance->BeginGame();
			GameInstance->ManageLevelPickups();
			ChangeWorlds(false);
		}

		else
		{
			GameInstance->ManageLevelPickups();
			ToggleAll();
		}
	}

}

void AWorldSwitchingGameModeBase::ChangeWorldsProxy()
{
	ChangeWorlds();

	/// Switches between the head and the mask
	PlayerPawn->SwitchingHead();
}

void AWorldSwitchingGameModeBase::ChangeWorlds(bool bShowTransitionEffects)
{
	if (bSphereIsRunning) return;

	bIsSpiritWorld = !bIsSpiritWorld;
	
	if (TestWorldChangeOverlaps()) return;
		
	PlayerPawn->bIsSpiritWorld = bIsSpiritWorld;

	if (bShowTransitionEffects) WorldTransitionEffects();
	
	ToggleAll();
}

void AWorldSwitchingGameModeBase::ToggleAll()
{
	TogglePhysicalWorldActors();
	ToggleSpiritWorldActors();
	TogglePhysicalSpiritActors();
	ToggleParticleEffects();
	ToggleLastingCameraEffects();
	ToggleInstanceMeshes();
}

bool AWorldSwitchingGameModeBase::TestWorldChangeOverlaps()
{
		//Momentarily test for overlap with Actors

	if (bIsSpiritWorld)
	{
		PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
		ToggleSpiritWorldActors();
	}
	else
	{
		PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
		TogglePhysicalWorldActors();
	}

	OtherActorPhysicalTest = PlayerPawn->GetOtherActorForPhysicalTest();

	if (OtherActorPhysicalTest && !OtherActorPhysicalTest->IsA(AArtifacts::StaticClass()))
	{

			/// UE_LOG(LogTemp, Warning, TEXT("Overlapped with PhysicalActor %s"), *OtherActorPhysicalTest->GetActorLabel())
			UGameplayStatics::PlaySound2D(GetWorld(), CannotReturnToPhysical);
			DeniedPhysicalReentryEffects();

			if (bIsSpiritWorld)				{

				UE_LOG(LogTemp, Warning, TEXT("GAME MODE: DETECTED SPIRIT OVERLAP"))

				//Stuck in Physical World
				bIsSpiritWorld = false;
				ToggleSpiritWorldActors();
				PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
			}

			else
			{
				UE_LOG(LogTemp, Warning, TEXT("GAME MODE: DETECTED PHYSICAL OVERLAP"))

				//Stuck in Spirit World
				bIsSpiritWorld = true;
				TogglePhysicalWorldActors();
				PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
			}

			LightUpCollidingActor();
			return true;
		}
	UE_LOG(LogTemp, Warning, TEXT("Did NOT Overlap with PhysicalActor"));

	PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	return false;
}


void AWorldSwitchingGameModeBase::GatherLevelCameras()
{
	for (TActorIterator<ALevelCamera> CameraItr(GetWorld()); CameraItr; ++CameraItr)
	{
		ALevelCamera* Camera = *CameraItr;

		if (Camera->CameraType == ECameraActor::FreeLevel)
			FreeLevelCamera = Camera;
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
			///SpiritItr->SetActorHiddenInGame(false);

			SpiritItr->FireFlies->Deactivate();

			for (int MaterialIndex{ 0 }; MaterialIndex < SpiritEnemySpiritWorldMaterials.Num(); ++MaterialIndex)
			{
				SpiritItr->GetMesh()->SetMaterial(MaterialIndex, SpiritEnemySpiritWorldMaterials[MaterialIndex]);
			}
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
			///SpiritItr->SetActorHiddenInGame(true);

			SpiritItr->FireFlies->Activate();

			for (int MaterialIndex{ 0 }; MaterialIndex < SpiritEnemyPhysicalWorldMaterials.Num(); ++MaterialIndex)
			{
				SpiritItr->GetMesh()->SetMaterial(MaterialIndex, SpiritEnemyPhysicalWorldMaterials[MaterialIndex]);
			}
			
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
					ParticleItr->PhysicalWorldParticles->SetHiddenInGame(true);
					ParticleItr->SpiritWorldParticles->Activate();
					ParticleItr->SpiritWorldParticles->SetHiddenInGame(false);
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
					ParticleItr->PhysicalWorldParticles->SetHiddenInGame(false);
					ParticleItr->SpiritWorldParticles->Deactivate();
					ParticleItr->SpiritWorldParticles->SetHiddenInGame(true);
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

void AWorldSwitchingGameModeBase::TogglePhysicalSpiritActors()
{
	if (bIsSpiritWorld)
	{
		for (TActorIterator<APSWorldActor> PSActorItr(GetWorld()); PSActorItr; ++PSActorItr)
		{
			APSWorldActor *PSActor = *PSActorItr;

			if (PSActor->WorldChangeType == EWorldChangeType::None) continue;

			if (PSActor->WorldChangeType == EWorldChangeType::ToggleComponents)
			{
				PSActor->MeshPhysical->SetHiddenInGame(true);
				PSActor->MeshSpirit->SetHiddenInGame(false);
			}

			if (PSActor->WorldChangeType == EWorldChangeType::ChangeMaterials)
			{
				PSActor->AssignSpiritMaterials();
			}
		}

		for (TActorIterator<AAltar> AltarItr(GetWorld()); AltarItr; ++AltarItr)
		{
			AAltar *Altar = *AltarItr;

			Altar->ActivateSpiritGoddess();
			
		}


	}
	else
	{
		for (TActorIterator<APSWorldActor> PSActorItr(GetWorld()); PSActorItr; ++PSActorItr)
		{
			APSWorldActor *PSActor = *PSActorItr;

			if (PSActor->WorldChangeType == EWorldChangeType::None) continue;

			if (PSActor->WorldChangeType == EWorldChangeType::ToggleComponents)
			{
				PSActor->MeshPhysical->SetHiddenInGame(false);
				PSActor->MeshSpirit->SetHiddenInGame(true);
			}

			if (PSActor->WorldChangeType == EWorldChangeType::ChangeMaterials)
			{
				PSActor->AssignPhysicalMaterials();
			}
		}
		for (TActorIterator<AAltar> AltarItr(GetWorld()); AltarItr; ++AltarItr)
		{
			AAltar *Altar = *AltarItr;
			Altar->ActivatePhysicalGoddess();
		}

	}
}

//Siden det er begrenset hvor mange meshes vi skal instantiate, setter jeg en if(bIsSpiritWorld) for hvert element
//i motsetning til de andre actorene som det kan bli mange flere av. Vil ikke teste midt i der.
void AWorldSwitchingGameModeBase::ToggleInstanceMeshes()
{
		int MeshIndexAccumulator = 0;

		for (int i = 0; i < Meshes.Num(); ++i)
		{
			if (NumberOfMaterials[i])
			MeshIndexAccumulator += NumberOfMaterials[i];

			int MaterialElementIterator = 0;
			for (int j = MeshIndexAccumulator - NumberOfMaterials[i]; j < MeshIndexAccumulator; ++j)
			{
				/// builden ville ikke tillate disse linjene så kommenterte det ut
				/*if (Meshes[i])
				{
					if (bIsSpiritWorld) Meshes[i]->SetMaterial(MaterialElementIterator, SpiritMaterials[j]);
					else Meshes[i]->SetMaterial(MaterialElementIterator, PhysicalMaterials[j]);
				}*/

				MaterialElementIterator++;
				UE_LOG(LogTemp, Warning, TEXT("ToggleInstanceMeshes: Assigning material index %i"), j)
			}
		}
}
