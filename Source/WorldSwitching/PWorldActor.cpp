// Fill out your copyright notice in the Description page of Project Settings.

#include "PWorldActor.h"
#include "WorldSwitchingGameModeBase.h"

// Sets default values
APWorldActor::APWorldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bOptOutOfCollisionChange = false;
	bOptOutOfVisibilityChange = false;
}

// Called when the game starts or when spawned
void APWorldActor::BeginPlay()
{
	Super::BeginPlay();
	
	GameModeRef = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode());

	if (bCanBeSensed) TriggerSetMeshRef();
	if (MeshRef)
	{
		GetOriginalMaterials();
	}
}

// Called every frame
void APWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Runs in BeginPlay if bCanBeSensend
void APWorldActor::GetOriginalMaterials()
{
	NumberOfMaterials = MeshRef->GetNumMaterials();
	OriginalMaterial.Init(nullptr, NumberOfMaterials);
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		OriginalMaterial[i] = MeshRef->GetMaterial(i);
	}
}

void APWorldActor::TimeLineLightUp(float TimeLine)
{
	for (int i = 0; i < NumberOfMaterials; ++i)
	{

		DynamicMaterials[i]->SetScalarParameterValue("Opacity", TimeLine);
	}
}

void APWorldActor::ReapplyOriginalMaterials()
{
	SetActorHiddenInGame(true);
	GameModeRef->DecrementLitUpBySensing();
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		MeshRef->SetMaterial(i, OriginalMaterial[i]);
	}
	
}

void APWorldActor::LightUpActorWhenSensed()
{
	GameModeRef->IncrementLitUpBySensing();
	ApplyDynamicMaterials();
	SetActorHiddenInGame(false);

	//Forer timeline inn i TimeLineLightUp, og kjører reapplyOriginalMaterials når ferdig
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		if (DynamicMaterials[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("DYNAMIC MATERIALS IS NOT NULL"))
		}
	}

	TimeLineLightUpTrigger();


}

void APWorldActor::ApplyDynamicMaterials()
{
	DynamicMaterials.Init(nullptr, NumberOfMaterials);

	//Set all materal elements to dummy material
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		MeshRef->SetMaterial(i, DummyLightUpMaterial);
	}


	//MakeDynamicMaterialInstance of Dummy materials
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		DynamicMaterials[i] = MeshRef->CreateDynamicMaterialInstance(i);
		if (DynamicMaterials[i])UE_LOG(LogTemp, Warning, TEXT("DYNAMIC MATERIAL CREATED"))
	}
}


