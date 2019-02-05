// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameModeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"

class ASpiritTest;

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

	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpiritTest::StaticClass(), EnemySpirits);
	

	if (CameraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got a CameraComponent"))
	}

	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got first PlayerController"))

			PlayerController->InputComponent->BindAction("ChangeWorlds", IE_Pressed, this, &AWorldSwitchingGameModeBase::ChangeWorlds);
	}

	
	//https://wiki.unrealengine.com/Iterators:_Object_%26_Actor_Iterators,_Optional_Class_Scope_For_Faster_Search
	for (TActorIterator<ASpiritTest> SpiritItr(GetWorld()); SpiritItr; ++SpiritItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ASpiritTest *SpiritTest = *SpiritItr;
		UE_LOG(LogTemp, Warning, TEXT("Found Spirit!"))
		
		SpiritItr->SetActorEnableCollision(false);
		SpiritItr->SetActorHiddenInGame(true);

	}

	//Alle Spiritmesh starter som ikke-synlig og ikke-collision
	for (TActorIterator<AActor> SpiritMeshItr(GetWorld()); SpiritMeshItr; ++SpiritMeshItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AActor *SpiritTest = *SpiritMeshItr;
		if (SpiritMeshItr->ActorHasTag("VisibleInSpirit"))
		{
			UE_LOG(LogTemp,Warning, TEXT("Tag found"))
			SpiritMeshItr->SetActorEnableCollision(false);
			SpiritMeshItr->SetActorHiddenInGame(true);
		}
	}
}


void AWorldSwitchingGameModeBase::ChangeWorlds()
{
	WorldTransitionEffects();

	bIsSpiritWorld = !bIsSpiritWorld;
	

	if (bIsSpiritWorld)
	{
		CameraComponent->PostProcessSettings.VignetteIntensity = 1.0f;

		//Finne fienden
		for (TActorIterator<ASpiritTest> SpiritItr(GetWorld()); SpiritItr; ++SpiritItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ASpiritTest *SpiritTest = *SpiritItr;
			SpiritItr->SetActorEnableCollision(true);
			SpiritItr->SetActorHiddenInGame(false);
		}

		//Finne alle meshes som kun er aktiv i SpiritWorld
		for (TActorIterator<AActor> SpiritMeshItr(GetWorld()); SpiritMeshItr; ++SpiritMeshItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AActor *SpiritTest = *SpiritMeshItr;
			if (SpiritMeshItr->ActorHasTag("VisibleInSpirit"))
			{
				SpiritMeshItr->SetActorEnableCollision(true);
				SpiritMeshItr->SetActorHiddenInGame(false);
			}
		}
	}

	else

	{
		if (CameraComponent)
		{
			CameraComponent->PostProcessSettings.VignetteIntensity = 0.0f;
		}

		for (TActorIterator<ASpiritTest> SpiritItr(GetWorld()); SpiritItr; ++SpiritItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ASpiritTest *SpiritTest = *SpiritItr;
			SpiritItr->SetActorEnableCollision(false);
			SpiritItr->SetActorHiddenInGame(true);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Etter Vignette;"))

		
}