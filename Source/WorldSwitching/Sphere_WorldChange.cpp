// Fill out your copyright notice in the Description page of Project Settings.

#include "Sphere_WorldChange.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASphere_WorldChange::ASphere_WorldChange()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));

	RootComponent = BallMesh;

	
}

// Called when the game starts or when spawned
void ASphere_WorldChange::BeginPlay()
{
	Super::BeginPlay();
	//DynamicMaterial = BallMesh->CreateDynamicMaterialInstance(0);

	GameModeRef = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode());

	if(GameModeRef)
	bIsSpiritWorld = GameModeRef->bIsSpiritWorld;

	if (GameModeRef && bIsSpiritWorld)
	{
		StartScale = FVector(1.f, 1.f, 1.f);
		EndScale = FVector(300.f, 300.f, 300.f);
	}
	else if (GameModeRef && !bIsSpiritWorld)
	{
		StartScale = FVector(300.f, 300.f, 300.f);
		EndScale = FVector(1.f, 1.f, 1.f);
	}
	
	//Fade(DynamicMaterial);
	TriggerMorph(bIsSpiritWorld);

}

// Called every frame
void ASphere_WorldChange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UFUNCTION(BlueprintCallable)
void ASphere_WorldChange::Morph(float TimeLine)
{
	SetActorScale3D(UKismetMathLibrary::VLerp(StartScale, EndScale, TimeLine));
}

