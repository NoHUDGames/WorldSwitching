// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_Portal.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Classes/Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


EPortalIndex APS_Portal::PlayerCameFrom = EPortalIndex::HubLevel_1;

EComingOrGoing  APS_Portal::ComingOrGoing;

APS_Portal::APS_Portal()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh2"));
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Boxtrigger"));
	CharacterInner = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterInner"));
	CharacterOuter = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterOuter"));
	CameraInner = CreateDefaultSubobject<USceneComponent>(TEXT("CameraInner"));
	CameraOuter = CreateDefaultSubobject<USceneComponent>(TEXT("CameraOuter"));
	PortalLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PortalLight"));
	Mesh2->SetupAttachment(RootComponent);
	BoxTrigger->SetupAttachment(RootComponent);
	CharacterInner->SetupAttachment(RootComponent);
	CharacterOuter->SetupAttachment(RootComponent);
	CameraInner->SetupAttachment(RootComponent);
	CameraOuter->SetupAttachment(RootComponent);
	PortalLight->SetupAttachment(RootComponent);
}

void APS_Portal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APS_Portal::BeginPlay()
{
	Super::BeginPlay();

	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &APS_Portal::ExitLevelSequence);
	GameInstance = Cast<UWorldSwitchingGameInstance>(GetGameInstance());
	GameMode = Cast<AWorldSwitchingGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerPawn = Cast<ABP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	PlayerCamera = Cast<UCameraComponent>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass(UCameraComponent::StaticClass()));

	
	if (!bBeginActivated)
	{
		PortalLight->SetIntensity(0);
		BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else	Activate();

	if (GameInstance->GetAltarArtifacts() >= ArtifactsNeededToUse) Activate();

	for (TActorIterator<ACameraActor> PortalEffectCamera(GetWorld()); PortalEffectCamera; ++PortalEffectCamera)
	{
		ACameraActor* Temp = *PortalEffectCamera;
		if (Temp->GetActorLabel() == "LevelFreeCamera")
		{
			LevelCamera = Temp;
			UE_LOG(LogTemp, Warning, TEXT("Found camera name: %s"), *LevelCamera->GetActorLabel())
		}
	}

	// Enter Level through Portal
	CurrentMapName = GetWorld()->GetMapName();
	if (!GameInstance->GetbIsFirstTimeStartingGame() && SetPortalToEnterFrom())
	{
		ComingOrGoing = COMING;
		PortalToEnterFrom->BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetupNavigationPoints();
		PreparePlayerAndCamera();
		FadeCamera(true, LevelCamera);
		TL_TriggerMovePlayerIntoPortal();

		GetWorldTimerManager().SetTimer(ActivatePlayerHandle, this, &APS_Portal::ActivatePlayerAfterEntry, 3.f, false);

		TL_TriggerEnterLevelSequence();
	}
	// Enter Level through Portal
}

void APS_Portal::FadeCameraProxy()
{
	FadeCamera(false, LevelCamera);
}

void APS_Portal::TL_MovePlayerIntoPortal(float MovePlayer, float MoveCamera)
{

	PlayerPawn->AddMovementInput(PlayerLookAt.Vector(), MovePlayer);

	if (ComingOrGoing == GOING)
	LevelCamera->SetActorLocation(FMath::Lerp(CameraOuterLoc, CameraInnerLoc, MoveCamera
			));

	else if (ComingOrGoing == COMING)

	LevelCamera->SetActorLocation(FMath::Lerp(CameraInnerLoc, CharacterOuterLoc, MoveCamera
			));

}

void APS_Portal::Activate(bool WithOpeningSequence)
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	AParticleEffectActor* Particle = GetWorld()->SpawnActor<AParticleEffectActor>(
		ParticleEffectToSpawn, SpawnLocation, SpawnRotation);
	Particle->SpiritWorldParticles->Activate();

	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bIsActive = true;
	PortalLight->SetIntensity(20000);
}
void APS_Portal::TL_EnterLevelSequence(float CameraTimeLine, float PlayerTimel, float FadeCamera)
{
	

}

void APS_Portal::ExitLevelSequence(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult &SweepResult)
{
	ComingOrGoing = GOING;
	
	PlayerCameFrom = PortalIndex;

	if (GameMode && !GameMode->bIsSpiritWorld) return;
	else SetActorEnableCollision(false);

	if (PlayerPawn && GameInstance && LevelCamera)
	{
		GameInstance->SetPlayerHealth(PlayerPawn->GetLives());
		GameInstance->SetPlayerArtifacts(PlayerPawn->GetArtifacts());

		SetupNavigationPoints();
		PreparePlayerAndCamera();
		
	}
	UGameplayStatics::PlaySound2D(GetWorld(), PortalEnterSound);


	GetWorldTimerManager().SetTimer(CameraFadeHandle, this, &APS_Portal::FadeCameraProxy, 1.f, false);
	GetWorldTimerManager().SetTimer(PlayerMoveHandle, this, &APS_Portal::TL_TriggerMovePlayerIntoPortal, 1.1f, false);


	GetWorldTimerManager().SetTimer(ExitHandle, this, &APS_Portal::ExitLevel, 5.f, false);
	
}

void APS_Portal::ExitLevel()
{
	switch (DestinationLevel)
	{
	case EDestinationLevel::Level_1:
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Levels/GameLevels/Level_1"), TRAVEL_Absolute);
		break;
	case EDestinationLevel::Level_2:
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Levels/GameLevels/Level_2"), TRAVEL_Absolute);
		break;
	case EDestinationLevel::Hub:
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Levels/GameLevels/Hub"), TRAVEL_Absolute);

	}
}

bool APS_Portal::SetPortalToEnterFrom()
{
	APS_Portal* Temp = nullptr;
	if (CurrentMapName.Contains("Hub"))
	{
		if (PlayerCameFrom == EPortalIndex::Level_1Entrance || PlayerCameFrom == EPortalIndex::Level_1Exit)
		{
			if (PortalIndex == EPortalIndex::HubLevel_1)
			{
				Temp = this;
				if (Temp)
				{
					PortalToEnterFrom = Temp;
					UE_LOG(LogTemp, Warning, TEXT("HUB: GOT PORTALT TO ENTER FROM: %s"), *PortalToEnterFrom->GetActorLabel())
					return true; }
			}
		}

		if (PlayerCameFrom == EPortalIndex::Level_2Entrance || PlayerCameFrom == EPortalIndex::Level_2Exit)
		{
			if (PortalIndex == EPortalIndex::HubLevel_2)
			{
				Temp = this;
				if (Temp)
				{
					PortalToEnterFrom = Temp;
					UE_LOG(LogTemp, Warning, TEXT("HUB: GOT PORTALT TO ENTER FROM: %s"), *PortalToEnterFrom->GetActorLabel())
						return true;
				}
			}
		}
	}

	if (CurrentMapName.Contains("1"))
	{
		if (PortalIndex == EPortalIndex::Level_1Entrance)
		{
			Temp = this;
			if (Temp)
			{
				PortalToEnterFrom = Temp;
				UE_LOG(LogTemp, Warning, TEXT("Level1: GOT PORTAL TO ENTER FROM: %s"), *PortalToEnterFrom->GetActorLabel())
					return true; }}}

	if (CurrentMapName.Contains("2"))
	{
		if (PortalIndex == EPortalIndex::Level_2Entrance)
		{
			Temp = this;
			if (Temp)
			{
				PortalToEnterFrom = Temp;
				UE_LOG(LogTemp, Warning, TEXT("Level2: GOT PORTAL TO ENTER FROM: %s"), *PortalToEnterFrom->GetActorLabel())
					return true; }}}

	return false;
}

void APS_Portal::SetupNavigationPoints()
{
	//ComingOrGoing is static

	if (ComingOrGoing == COMING)
	{
		CameraInnerLoc = PortalToEnterFrom->CameraInner->GetComponentLocation();
		CameraOuterLoc = PortalToEnterFrom->CameraOuter->GetComponentLocation();
		CharacterInnerLoc = PortalToEnterFrom->CharacterInner->GetComponentLocation();
		CharacterOuterLoc = PortalToEnterFrom->CharacterOuter->GetComponentLocation();
	}

	else if (ComingOrGoing == GOING)
	{
		CameraInnerLoc = CameraInner->GetComponentLocation();
		CameraOuterLoc = CameraOuter->GetComponentLocation();
		CharacterInnerLoc = CharacterInner->GetComponentLocation();
		CharacterOuterLoc = CharacterOuter->GetComponentLocation();

	}
	
}

void APS_Portal::PreparePlayerAndCamera()
{
	//ComingOrGoing is static

	if (ComingOrGoing == COMING)
	{
		PlayerLookAt = UKismetMathLibrary::FindLookAtRotation(
			CameraInnerLoc, CameraOuterLoc);
		
		LevelCamera->SetActorLocation(CameraInnerLoc);
		PlayerPawn->SetActorLocation(CharacterInnerLoc);
		Cast<APlayerController>(PlayerPawn->GetController())->SetViewTarget(LevelCamera);
		

	}

	else if (ComingOrGoing == GOING)
	{
		PlayerLookAt = UKismetMathLibrary::FindLookAtRotation(
			CameraOuterLoc, CameraInnerLoc);

		LevelCamera->SetActorLocation(CameraOuterLoc);
		Cast<APlayerController>(PlayerPawn->GetController())->SetViewTargetWithBlend(LevelCamera, 1.f);
	}

	
	LevelCamera->SetActorRotation(PlayerLookAt);

	PlayerPawn->DisableInput(0);
	PlayerPawn->SetActorTickEnabled(false);
	PlayerPawn->bUseControllerRotationYaw = false;
	PlayerPawn->SetActorRotation(PlayerLookAt);
}

void APS_Portal::ActivatePlayerAfterEntry()
{
	PlayerPawn->EnableInput(0);
	PlayerPawn->SetActorTickEnabled(true);
	PlayerPawn->bUseControllerRotationYaw = true;
	Cast<APlayerController>(PlayerPawn->GetController())->SetViewTargetWithBlend(PlayerPawn, 2);

	PortalToEnterFrom->BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}