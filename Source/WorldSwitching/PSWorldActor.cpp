// Fill out your copyright notice in the Description page of Project Settings.

#include "PSWorldActor.h"

// Sets default values
APSWorldActor::APSWorldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceenRoot"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = SceneRoot;

	Mesh->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void APSWorldActor::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = Mesh->CreateDynamicMaterialInstance(0);

	if (DynamicMaterial)
	{
		PhysicalColor = DynamicMaterial->K2_GetVectorParameterValue("BaseColor");
		PhysicalGlow = DynamicMaterial->K2_GetScalarParameterValue("Glow");
		PhysicalOpacity = DynamicMaterial->K2_GetScalarParameterValue("Opacity");
	}
}

// Called every frame
void APSWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APSWorldActor::ActivatePhysicalMaterialProperties()
{
	DynamicMaterial->SetVectorParameterValue("BaseColor", PhysicalColor);
	DynamicMaterial->SetScalarParameterValue("Glow", PhysicalGlow);
	DynamicMaterial->SetScalarParameterValue("Opacity", PhysicalOpacity);
}

void APSWorldActor::ActivateSpiritMaterialProperties()
{
	DynamicMaterial->SetVectorParameterValue("BaseColor", SpiritColor);
	DynamicMaterial->SetScalarParameterValue("Glow", SpiritGlow);
	DynamicMaterial->SetScalarParameterValue("Opacity", SpiritOpacity);
}
