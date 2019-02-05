// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameModeBase.h"
#include "Kismet/GameplayStatics.h"



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
}


void AWorldSwitchingGameModeBase::ChangeWorlds()
{
	WorldTransitionEffects();

	bIsSpiritWorld = !bIsSpiritWorld;
	

	if (bIsSpiritWorld)
	{
		CameraComponent->PostProcessSettings.VignetteIntensity = 1.0f;
	}

	else

	{
		if (CameraComponent)
		{
			CameraComponent->PostProcessSettings.VignetteIntensity = 0.0f;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Etter Vignette;"))
}