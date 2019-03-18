// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_Portal.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


APS_Portal::APS_Portal()
{
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


}

void APS_Portal::Tick(float DeltaTime)
{

}

void APS_Portal::BeginPlay()
{
	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &APS_Portal::Travel);
	GameInstance = Cast<UWorldSwitchingGameInstance>(GetGameInstance());
	GameMode = Cast<AWorldSwitchingGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerPawn = Cast<ABP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (!bBeginActivated)
		BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	else	Activate();

	if (ArtifactsNeededToUse <= GameInstance->GetAltarArtifacts()) Activate();

	

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

void APS_Portal::Travel(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult &SweepResult)
{
	if (GameMode && GameMode->bIsSpiritWorld)
	{
		if (PlayerPawn && GameInstance)
		{
			GameInstance->SetPlayerHealth(PlayerPawn->GetLives());
			GameInstance->SetPlayerArtifacts(PlayerPawn->GetArtifacts());
		}
		
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
}