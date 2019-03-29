// Fill out your copyright notice in the Description page of Project Settings.

#include "SpringArmSensor.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	if (CheckForPSWorldActor(OtherActor) || CheckForPWorldActor(OtherActor) || CheckForSWorldActor(OtherActor))
	{
		FadeDownTrigger();
	}
}


bool ASpringArmSensor::CheckForPSWorldActor(AActor* OtherActor)
{
	if (OtherActor->IsA(AAltar::StaticClass())) return false;

	if (OtherActor->IsA(APSWorldActor::StaticClass()))
	{
		APSWorldActor* Actor = Cast<APSWorldActor>(OtherActor);

		if (Actor->MeshBoth)
		{
			int NumberOfMaterials = Actor->MeshBoth->GetNumMaterials();

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				OriginalMaterialsPrimary.Add(Actor->MeshBoth->GetMaterial(i));
			}
			
			Actor->MeshBoth->SetMaterial(0, DummyMaterial);

			DynamicMaterial = Actor->MeshBoth->CreateDynamicMaterialInstance(0);

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				Actor->MeshBoth->SetMaterial(i, DynamicMaterial);
			}
		}
		//If this is valid we know both physical and spirit meshcomponents have objects
		else if (Actor->MeshPhysical)
		{
			int NumberOfMaterials = Actor->MeshPhysical->GetNumMaterials();

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				OriginalMaterialsPrimary.Add(Actor->MeshPhysical->GetMaterial(i));
				OriginalMaterialsSecondary.Add(Actor->MeshPhysical->GetMaterial(i));
			}

			Actor->MeshPhysical->SetMaterial(0, DummyMaterial);

			DynamicMaterial = Actor->MeshPhysical->CreateDynamicMaterialInstance(0);

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				Actor->MeshPhysical->SetMaterial(i,DynamicMaterial);
				Actor->MeshSpirit->SetMaterial(i, DynamicMaterial);
			}
		}
		
		if (DynamicMaterial) return true;
		else return false;
	}
	return false;
}

bool ASpringArmSensor::CheckForPWorldActor(AActor* OtherActor)
{
	
	if (OtherActor->IsA(APWorldActor::StaticClass()))
	{
		APWorldActor* Actor = Cast<APWorldActor>(OtherActor);

		if (Actor->MeshRef)
		{
			int NumberOfMaterials = Actor->MeshRef->GetNumMaterials();

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				OriginalMaterialsPrimary.Add(Actor->MeshRef->GetMaterial(i));
			}

			Actor->MeshRef->SetMaterial(0, DummyMaterial);

			DynamicMaterial = Actor->MeshRef->CreateDynamicMaterialInstance(0);

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				Actor->MeshRef->SetMaterial(i, DynamicMaterial);
			}

			if (DynamicMaterial) return true;
			else return false;

		}
		return false;
	}
	else return false;

}

bool ASpringArmSensor::CheckForSWorldActor(AActor* OtherActor)
{

	if (OtherActor->IsA(ASWorldActor::StaticClass()))
	{
		ASWorldActor* Actor = Cast<ASWorldActor>(OtherActor);

		if (Actor->MeshRef)
		{
			int NumberOfMaterials = Actor->MeshRef->GetNumMaterials();

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				OriginalMaterialsPrimary.Add(Actor->MeshRef->GetMaterial(i));
			}

			Actor->MeshRef->SetMaterial(0, DummyMaterial);

			DynamicMaterial = Actor->MeshRef->CreateDynamicMaterialInstance(0);

			for (int i = 0; i < NumberOfMaterials; ++i)
			{
				Actor->MeshRef->SetMaterial(i, DynamicMaterial);
			}

			if (DynamicMaterial) return true;
			else return false;

		}
		return false;
	}
	else return false;

}

void ASpringArmSensor::FadeDown(float TimeLine)
{
	DynamicMaterial->SetScalarParameterValue("Opacity", UKismetMathLibrary::Lerp(
		FadeDownStart, FadeDownEnd, TimeLine)
	);
}

void ASpringArmSensor::FadeUp(float TimeLine)
{
	DynamicMaterial->SetScalarParameterValue("Opacity", UKismetMathLibrary::Lerp(
		FadeUpStart, FadeUpEnd, TimeLine)
	);
}