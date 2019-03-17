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


	if (!bBeginActivated)
		BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	else
		ActivatePortal();

	GameInstance = Cast<UWorldSwitchingGameInstance>(GetGameInstance());
	PlayerPawn = Cast<ABP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void APS_Portal::ActivatePortal()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	GetWorld()->SpawnActor<AParticleEffectActor>(ParticleEffectToSpawn, SpawnLocation, SpawnRotation);
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

void APS_Portal::Travel(UPrimitiveComponent* OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult &SweepResult)
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