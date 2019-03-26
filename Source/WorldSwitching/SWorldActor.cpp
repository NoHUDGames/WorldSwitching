// Fill out your copyright notice in the Description page of Project Settings.

#include "SWorldActor.h"
#include "WorldSwitchingGameModeBase.h"

// Sets default values
ASWorldActor::ASWorldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bOptOutOfCollisionChange = false;
	bOptOutOfVisibilityChange = false;
	bCanBeSensed = false;
}


// Called when the game starts or when spawned
void ASWorldActor::BeginPlay()
{
	Super::BeginPlay();
	GameModeRef = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode());

	if (bCanBeSensed) TriggerSetMeshRef();
	if (MeshRef)
	{
		GetOriginalMaterials();
	}
}

//Runs in BeginPlay if bCanBeSensend
void ASWorldActor::GetOriginalMaterials()
{
	NumberOfMaterials = MeshRef->GetNumMaterials();
	OriginalMaterial.Init(nullptr, NumberOfMaterials);
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		OriginalMaterial[i] = MeshRef->GetMaterial(i);
	}
}

// Called every frame
void ASWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWorldActor::TimeLineLightUp(float TimeLine)
{
	for (int i = 0; i < NumberOfMaterials; ++i)
	{

		DynamicMaterials[i]->SetScalarParameterValue("Opacity", TimeLine);
	}
}

void ASWorldActor::ReapplyOriginalMaterials()
{
	SetActorHiddenInGame(true);
	GameModeRef->DecrementLitUpBySensing();
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		MeshRef->SetMaterial(i, OriginalMaterial[i]);
	}
	
}

void ASWorldActor::LightUpActorWhenSensed()
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

void ASWorldActor::ApplyDynamicMaterials()
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
