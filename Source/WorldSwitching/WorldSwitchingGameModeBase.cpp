// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameModeBase.h"
#include "PWorldActor.h"
#include "SWorldActor.h"
#include "PSWorldActor.h"
#include "SpiritTest.h"
#include "PWorldCharacter.h"
#include "ParticleEffectActor.h"
#include "Artifacts.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
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
	CurrentWorld = GetWorld();
	CurrentMapName = CurrentWorld->GetMapName();
	SetLandscapesReferences();
	SetFoliageMaterialParameterCollectionReferences();

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
			PlayerController->InputComponent->BindAction("UnlockAllAbilities", IE_Pressed, this, &AWorldSwitchingGameModeBase::UnlockAllAbilities);
	}
	
	if (GameInstance)
	{
		if (GameInstance->GetbIsFirstTimeStartingGame())
		{
			GameInstance->SetbIsFirstTimeStartingGame(false);

			if (CurrentMapName.Contains("2") || CurrentMapName.Contains("Hub"))
			{
				GameInstance->bIsDashingLocked = false;
				GameInstance->bIsWorldChangingLocked = false;
				GameInstance->bIsSensingLocked = false;
			}

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

//CalledfromInput
void AWorldSwitchingGameModeBase::ChangeWorldsProxy()
{
	if (!GameInstance->bIsWorldChangingLocked)
	{
		ChangeWorlds();

		/// Switches between the head and the mask
		PlayerPawn->SwitchingHead();
	}
}

void AWorldSwitchingGameModeBase::ChangeWorlds(bool bShowTransitionEffects)
{
	if (bSphereIsRunning || bIsLightUpEffectRunning) return;

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
	ToggleFoliageMaterialProperties();
	ToggleLandscapes();
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

	if (OtherActorPhysicalTest && !OtherActorPhysicalTest->IsA(AArtifacts::StaticClass()) && !OtherActorPhysicalTest->IsA(AS_PickupShield::StaticClass()))
	{

			/// UE_LOG(LogTemp, Warning, TEXT("Overlapped with PhysicalActor %s"), *OtherActorPhysicalTest->GetActorLabel())
			UGameplayStatics::PlaySound2D(GetWorld(), CannotReturnToPhysical);
			DeniedPhysicalReentryEffects();

			if (bIsSpiritWorld)				{

				UE_LOG(LogTemp, Warning, TEXT("GAME MODE: DETECTED SPIRIT OVERLAP"))

				//Stuck in physical world
				bIsSpiritWorld = false;
				ToggleSpiritWorldActors();
				PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
			}

			else
			{
				UE_LOG(LogTemp, Warning, TEXT("GAME MODE: DETECTED PHYSICAL OVERLAP"))
				bIsSpiritWorld = true;
				TogglePhysicalWorldActors();
				PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
			}

			if (LitUpBySensing < 1) LightUpCollidingActor(OtherActorPhysicalTest);
			

			return true;
		}
	UE_LOG(LogTemp, Warning, TEXT("Did NOT Overlap with PhysicalActor"));

	PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	PlayerCapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	return false;
}

void AWorldSwitchingGameModeBase::LightUpCollidingActor(AActor* CollidingActor)
{
	if (bIsLightUpEffectRunning) return;
	bIsLightUpEffectRunning = true;

	m_CollidingActor = CollidingActor;

	MeshComp = m_CollidingActor->FindComponentByClass<UStaticMeshComponent>();

	NumberOfMaterials = MeshComp->GetNumMaterials();

	//Extract original materials
	for (int i = 0; i < NumberOfMaterials; ++i)
		OriginalMaterials.Add(MeshComp->GetMaterial(i));

	//Apply temporary light up material
	for (int i = 0; i < NumberOfMaterials; ++i)
		MeshComp->SetMaterial(i, RedLightupMaterial);

	//CreateDynamicMaterials
	for (int i = 0; i < NumberOfMaterials; ++i)
		DynamicMaterials.Add(MeshComp->CreateDynamicMaterialInstance(i));

	m_CollidingActor->SetActorEnableCollision(false);
	m_CollidingActor->SetActorHiddenInGame(false);
	

	TL_TriggerActorLightup();

}

void AWorldSwitchingGameModeBase::FadeInOut(float TimeLine)
{

	for (int i = 0; i < NumberOfMaterials; ++i)
		DynamicMaterials[i]->SetScalarParameterValue("Opacity", TimeLine);
}

void AWorldSwitchingGameModeBase::CleanUp()
{
	for (int i = 0; i < NumberOfMaterials; ++i)
		MeshComp->SetMaterial(i, OriginalMaterials[i]);

	m_CollidingActor->SetActorHiddenInGame(true);
	NumberOfMaterials = 0;
	DynamicMaterials.Empty();
	OriginalMaterials.Empty();
	MeshComp = nullptr;
	m_CollidingActor = nullptr;
	bIsLightUpEffectRunning = false;
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
			PWorldChar->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

			for (int MaterialIndex{ 0 }; MaterialIndex < ShamanEnemySpiritWorldMaterials.Num(); ++MaterialIndex)
			{
				PWorldChar->GetMesh()->SetMaterial(MaterialIndex, ShamanEnemySpiritWorldMaterials[MaterialIndex]);
			}
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

			if (Cast<AArtifacts>(PWorldActor))
				Cast<AArtifacts>(PWorldActor)->SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		}

		for (TActorIterator<APWorldCharacter> PCharItr(GetWorld()); PCharItr; ++PCharItr)
		{
			APWorldCharacter* PWorldChar = *PCharItr;
			PWorldChar->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

			for (int MaterialIndex{ 0 }; MaterialIndex < ShamanEnemyPhysicalWorldMaterials.Num(); ++MaterialIndex)
			{
				PWorldChar->GetMesh()->SetMaterial(MaterialIndex, ShamanEnemyPhysicalWorldMaterials[MaterialIndex]);
			}
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
			SpiritItr->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

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
			SpiritItr->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

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
					ParticleItr->PhysicalWorldParticles->SetHiddenInGame(true);
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

void AWorldSwitchingGameModeBase::SetFoliageMaterialParameterCollectionReferences()
{
	FoliageTrunk1Parameters = LoadObject<UMaterialParameterCollection>(NULL, TEXT("MaterialParameterCollection'/Game/Meshes/Scenery/Materials/Foliage_Trunk1_Parameters.Foliage_Trunk1_Parameters'"),
		NULL, LOAD_None, NULL);
	FoliageTrunk2Parameters = LoadObject<UMaterialParameterCollection>(NULL, TEXT("MaterialParameterCollection'/Game/Meshes/Scenery/Materials/Foliage_Trunk2_Parameters.Foliage_Trunk2_Parameters'"),
		NULL, LOAD_None, NULL);
	FoliageLeaves1Parameters = LoadObject<UMaterialParameterCollection>(NULL, TEXT("MaterialParameterCollection'/Game/Meshes/Scenery/Materials/Foliage_Leaves1_Parameters.Foliage_Leaves1_Parameters'"),
		NULL, LOAD_None, NULL);
	FoliageLeaves2Parameters = LoadObject<UMaterialParameterCollection>(NULL, TEXT("MaterialParameterCollection'/Game/Meshes/Scenery/Materials/Foliage_Leaves2_Parameters.Foliage_Leaves2_Parameters'"),
		NULL, LOAD_None, NULL);


	if (FoliageTrunk1Parameters && FoliageTrunk2Parameters) UE_LOG(LogTemp, Warning, TEXT("GameMode: Got FoliageParameters"))
	else UE_LOG(LogTemp, Warning, TEXT("GameMode: DID NOT GET FoliageParameters"))


}

//This toggles visibility of different textures in materials applied to UStaticMeshes instanced out in the scene with foliage
void AWorldSwitchingGameModeBase::ToggleFoliageMaterialProperties()
{	
	if (!FoliageTrunk1Parameters || !FoliageTrunk1Parameters || !FoliageLeaves1Parameters || !FoliageLeaves2Parameters) return;

	if (bIsSpiritWorld)
	{	//Trunk1
		if (UKismetMathLibrary::RandomIntegerInRange(0, 1) < 1)
		{
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_P_Opacity", SpiritTrunk1[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_S1_Opacity", SpiritTrunk1[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_S2_Opacity", SpiritTrunk1[2]);
		}
		else
		{
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_P_Opacity", SpiritTrunk2[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_S1_Opacity", SpiritTrunk2[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_S2_Opacity", SpiritTrunk2[2]);
		}

		//Trunk2
		if (UKismetMathLibrary::RandomIntegerInRange(0, 1) < 1)
		{
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_P_Opacity", SpiritTrunk1[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_S1_Opacity", SpiritTrunk1[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_S2_Opacity", SpiritTrunk1[2]);
		}
		else
		{
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_P_Opacity", SpiritTrunk2[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_S1_Opacity", SpiritTrunk2[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_S2_Opacity", SpiritTrunk2[2]);
		}

		//Leaves 1 //FoliageLeaves1Parameters
		switch (UKismetMathLibrary::RandomIntegerInRange(0, 2))
		{
		case 0:
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_P_Opacity", SpiritLeaves1[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S1_Opacity", SpiritLeaves1[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S2_Opacity", SpiritLeaves1[2]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S3_Opacity", SpiritLeaves1[3]);
			break;
		case 1:
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_P_Opacity", SpiritLeaves2[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S1_Opacity", SpiritLeaves2[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S2_Opacity", SpiritLeaves2[2]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S3_Opacity", SpiritLeaves2[3]);
			break;
		case 2:
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_P_Opacity", SpiritLeaves3[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S1_Opacity", SpiritLeaves3[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S2_Opacity", SpiritLeaves3[2]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S3_Opacity", SpiritLeaves3[3]);
			break;
		default:
			break;
		}

		//Leaves 2 //FoliageLeaves2Parameters
		switch (UKismetMathLibrary::RandomIntegerInRange(0, 2))
		{
		case 0:
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_P_Opacity", SpiritLeaves1[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S1_Opacity", SpiritLeaves1[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S2_Opacity", SpiritLeaves1[2]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S3_Opacity", SpiritLeaves1[3]);
			break;
		case 1:
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_P_Opacity", SpiritLeaves2[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S1_Opacity", SpiritLeaves2[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S2_Opacity", SpiritLeaves2[2]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S3_Opacity", SpiritLeaves2[3]);
			break;
		case 2:
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_P_Opacity", SpiritLeaves3[0]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S1_Opacity", SpiritLeaves3[1]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S2_Opacity", SpiritLeaves3[2]);
			UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S3_Opacity", SpiritLeaves3[3]);
			break;
		default:
			break;
		}

	}

	else
	{
		//Trunk1
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_P_Opacity", PhysicalTrunk[0]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_S1_Opacity", PhysicalTrunk[1]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk1Parameters, "Trunk_S2_Opacity", PhysicalTrunk[2]);
		//Trunk2
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_P_Opacity", PhysicalTrunk[0]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_S1_Opacity", PhysicalTrunk[1]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageTrunk2Parameters, "Trunk_S2_Opacity", PhysicalTrunk[2]);
		//Leaves1
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_P_Opacity", PhysicalLeaves[0]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S1_Opacity", PhysicalLeaves[1]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S2_Opacity", PhysicalLeaves[2]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves1Parameters, "Leaves_S3_Opacity", PhysicalLeaves[3]);
		//Leaves2
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_P_Opacity", PhysicalLeaves[0]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S1_Opacity", PhysicalLeaves[1]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S2_Opacity", PhysicalLeaves[2]);
		UKismetMaterialLibrary::SetScalarParameterValue(CurrentWorld, FoliageLeaves2Parameters, "Leaves_S3_Opacity", PhysicalLeaves[3]);
	}


	//This is the previous stuff I did, before I discovered that Unreal doesnt like me trying to
	//Change materials on UStaticMeshes at runtime.
	/*
		int MeshIndexAccumulator = 0;

		for (int i = 0; i < Meshes.Num(); ++i)
		{
			if (NumberOfMaterials[i])
			MeshIndexAccumulator += NumberOfMaterials[i];

			int MaterialElementIterator = 0;
			for (int j = MeshIndexAccumulator - NumberOfMaterials[i]; j < MeshIndexAccumulator; ++j)
			{
				if (Meshes[i])
				{
					if (bIsSpiritWorld) Meshes[i]->SetMaterial(MaterialElementIterator, SpiritMaterials[j]);
					else Meshes[i]->SetMaterial(MaterialElementIterator, PhysicalMaterials[j]);
				}

				MaterialElementIterator++;
				UE_LOG(LogTemp, Warning, TEXT("ToggleInstanceMeshes: Assigning material index %i"), j)
			}
		}
		*/
}

void AWorldSwitchingGameModeBase::UnlockAllAbilities()
{
	GameInstance->bIsDashingLocked = false;
	GameInstance->bIsSensingLocked = false;
	GameInstance->bIsWorldChangingLocked = false;
}