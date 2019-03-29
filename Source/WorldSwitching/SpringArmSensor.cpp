// Fill out your copyright notice in the Description page of Project Settings.

#include "SpringArmSensor.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
ASpringArmSensor::ASpringArmSensor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	RootComponent = Collider;
}



// Called when the game starts or when spawned
void ASpringArmSensor::BeginPlay()
{
	Super::BeginPlay();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ASpringArmSensor::FadeDownProxy);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ASpringArmSensor::FadeUpProxy);
}

// Called every frame
void ASpringArmSensor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpringArmSensor::FadeUpProxy(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ASpringArmSensor::FadeDownProxy(UPrimitiveComponent * OverlappedComp, AActor* OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}


APSWorldActor* ASpringArmSensor::CheckForPSWorldActor(AActor* OtherActor)
{
	if (OtherActor->IsA(APSWorldActor::StaticClass()))
	{
		APSWorldActor* Actor = Cast<APSWorldActor>(OtherActor);

		if (Actor->MeshBoth)
		{
			int NumberOfMaterials = Actor->MeshBoth->GetNumMaterials();

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				Actor->MeshBoth->SetMaterial(i, DummyMaterial);
				DynamicMaterialsPrimary.Add(Actor->MeshBoth->CreateDynamicMaterialInstance(i));
			}
		}
		//If this is valid we know both physical and spirit meshcomponents have objects
		else if (Actor->MeshPhysical)
		{
			int NumberOfMaterials = Actor->MeshPhysical->GetNumMaterials();

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				Actor->MeshPhysical->SetMaterial(i, DummyMaterial);
				Actor->MeshSpirit->SetMaterial(i, DummyMaterial);

				DynamicMaterialsPrimary.Add(Actor->MeshPhysical->CreateDynamicMaterialInstance(i));
				DynamicMaterialsSecondary.Add(Actor->MeshSpirit->CreateDynamicMaterialInstance(i));
			}
		}
		return Actor;
	}
	else return nullptr;
}

void ASpringArmSensor::test()
{

}