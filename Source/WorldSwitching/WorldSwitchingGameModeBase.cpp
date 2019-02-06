// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameModeBase.h"
#include "Kismet/GameplayStatics.h"

class ASpiritTest;
class APWorldActor;

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

	
	//https://wiki.unrealengine.com/Iterators:_Object_%26_Actor_Iterators,_Optional_Class_Scope_For_Faster_Search
	for (TActorIterator<ASpiritTest> SpiritItr(GetWorld()); SpiritItr; ++SpiritItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ASpiritTest *SpiritTest = *SpiritItr;
		UE_LOG(LogTemp, Warning, TEXT("Found Spirit!"))
		
		SpiritItr->SetActorEnableCollision(false);
		SpiritItr->SetActorHiddenInGame(true);
	}
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
		ToggleSpiritCharacters();
		//For at kollisjonene ikke skal overlappe i samme tick


		//GetWorldTimerManager().SetTimer(SwitchLag, this,
		//&AWorldSwitchingGameModeBase::ToggleSpiritCharacters, 1.f, false);

	}

	else
	{
		if (CameraComponent)
		{
			CameraComponent->PostProcessSettings.VignetteIntensity = 0.0f;
		}

		//Først fjern spirits
		ToggleSpiritCharacters();
		TogglePWorldActors();
		
		//Deretter skru på fysiske omgivelser
		//GetWorldTimerManager().SetTimer(SwitchLag, this,
		//&AWorldSwitchingGameModeBase::TogglePWorldActors, 1.f, false);
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