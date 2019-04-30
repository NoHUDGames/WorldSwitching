// Fill out your copyright notice in the Description page of Project Settings.

#include "PSWorldActor.h"

// Sets default values
APSWorldActor::APSWorldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceenRoot"));
	MeshPhysical = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshPhysical"));
	MeshSpirit = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshSpirit"));
	MeshBoth = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshBoth"));

	RootComponent = SceneRoot;

	MeshPhysical->SetupAttachment(RootComponent);
	MeshSpirit->SetupAttachment(RootComponent);
	MeshBoth->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void APSWorldActor::BeginPlay()
{
	Super::BeginPlay();

	if (SpiritMaterials.Num() == PhysicalMaterials.Num()) NumberOfMaterials = SpiritMaterials.Num();
	else
	{
		///UE_LOG(LogTemp, Warning, TEXT("UNEQUAL NUMBER OF MATERIALS IN  %s"), *GetActorLabel())
		Destroy();
	}

	/*
	if (MeshBoth->GetStaticMesh())
	DynamicMaterial = MeshBoth->CreateDynamicMaterialInstance(0);

	if (DynamicMaterial)
	{
		PhysicalColor = DynamicMaterial->K2_GetVectorParameterValue("BaseColor");
		PhysicalGlow = DynamicMaterial->K2_GetScalarParameterValue("Glow");
		PhysicalOpacity = DynamicMaterial->K2_GetScalarParameterValue("Opacity");
	}
	*/
}

// Called every frame
void APSWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//Skal slettes
void APSWorldActor::ActivatePhysicalMaterialProperties()
{
	DynamicMaterial->SetVectorParameterValue("BaseColor", PhysicalColor);
	DynamicMaterial->SetScalarParameterValue("Glow", PhysicalGlow);
	DynamicMaterial->SetScalarParameterValue("Opacity", PhysicalOpacity);
}

//Skal slettes
void APSWorldActor::ActivateSpiritMaterialProperties()
{
	DynamicMaterial->SetVectorParameterValue("BaseColor", SpiritColor);
	DynamicMaterial->SetScalarParameterValue("Glow", SpiritGlow);
	DynamicMaterial->SetScalarParameterValue("Opacity", SpiritOpacity);
}

void APSWorldActor::AssignSpiritMaterials()
{
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		MeshBoth->SetMaterial(i, SpiritMaterials[i]);
	}

}
void APSWorldActor::AssignPhysicalMaterials()
{
	for (int i = 0; i < NumberOfMaterials; ++i)
	{
		MeshBoth->SetMaterial(i, PhysicalMaterials[i]);
	}

}