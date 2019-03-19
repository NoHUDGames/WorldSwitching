// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_Portal.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Classes/Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


APS_Portal::APS_Portal()
{
	PrimaryActorTick.bCanEverTick = true;

	TimeLineMovePortalCamera = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraMovementTimeline"));
	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh2"));
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Boxtrigger"));


	CharacterInner = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterInner"));

	CharacterOuter = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterOuter"));

	CameraInner = CreateDefaultSubobject<USceneComponent>(TEXT("CameraInner"));

	CameraOuter = CreateDefaultSubobject<USceneComponent>(TEXT("CameraOuter"));

	Mesh2->SetupAttachment(RootComponent);
	BoxTrigger->SetupAttachment(RootComponent);
	CharacterInner->SetupAttachment(RootComponent);
	CharacterOuter->SetupAttachment(RootComponent);
	CameraInner->SetupAttachment(RootComponent);
	CameraOuter->SetupAttachment(RootComponent);

	
	InterpFunction.BindUFunction(this, FName("MoveCameraIntoPortal"));
}

void APS_Portal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APS_Portal::BeginPlay()
{
	Super::BeginPlay();

	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &APS_Portal::TravelSequence);
	GameInstance = Cast<UWorldSwitchingGameInstance>(GetGameInstance());
	GameMode = Cast<AWorldSwitchingGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerPawn = Cast<ABP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (!bBeginActivated)
		BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	else	Activate();

	if (ArtifactsNeededToUse <= GameInstance->GetAltarArtifacts()) Activate();

	for (TActorIterator<ACameraActor> PortalEffectCamera(GetWorld()); PortalEffectCamera; ++PortalEffectCamera)
	{
		ACameraActor* Temp = *PortalEffectCamera;

		if (Temp->GetActorLabel() == "LevelFreeCamera")
		{
			LevelCamera = Temp;
			UE_LOG(LogTemp, Warning, TEXT("Found camera name: %s"), *LevelCamera->GetActorLabel())
		}
	}

	CameraPointsLookAt = UKismetMathLibrary::FindLookAtRotation(
		CameraOuter->GetComponentLocation(), CameraInner->GetComponentLocation());

	TimeLineMovePortalCamera->AddInterpFloat(CameraPortalMovement, InterpFunction, FName("Alpha"));
	CameraOuterLocation = CameraOuter->GetComponentLocation();
	CameraInnerLocation = CameraInner->GetComponentLocation();

	//UE_LOG(LogTemp, Warning, TEXT("Curvefloat at 2 sec: %f"), *CameraPortalMovement->GetFloatValue(2));
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

}

void APS_Portal::TravelSequence(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult &SweepResult)
{
	if (GameMode && !GameMode->bIsSpiritWorld) return;

	if (PlayerPawn && GameInstance && LevelCamera)
	{
		GameInstance->SetPlayerHealth(PlayerPawn->GetLives());
		GameInstance->SetPlayerArtifacts(PlayerPawn->GetArtifacts());
		PlayerPawn->DisableInput(0);
		PlayerPawn->SetActorTickEnabled(false);

		LevelCamera->SetActorLocation(CameraOuter->GetComponentLocation());

		LevelCamera->SetActorRotation(CameraPointsLookAt);

		Cast<APlayerController>(PlayerPawn->GetController())->SetViewTargetWithBlend(LevelCamera, 1.f);
	}

	GetWorldTimerManager().SetTimer(CameraMoveHandle, this, &APS_Portal::MoveCameraProxy, 1.1f, false);
	GetWorldTimerManager().SetTimer(ExitHandle, this, &APS_Portal::ExitLevel, 3.f, false);
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


void APS_Portal::MoveCameraIntoPortal(float value)
{
	UE_LOG(LogTemp, Warning, TEXT("FLOAT VALUE: %f"), value)
	LevelCamera->SetActorLocation(
		FMath::Lerp(
			CameraOuterLocation, CameraInnerLocation, value));
}

void APS_Portal::MoveCameraProxy()
{
	UE_LOG(LogTemp, Warning, TEXT("INSIDE MOVECAMERAPROXY"))
	TimeLineMovePortalCamera->PlayFromStart();
}
