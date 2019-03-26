// Fill out your copyright notice in the Description page of Project Settings.

#include "SensingSphere.h"
#include "WorldSwitchingGameModeBase.h"


// Sets default values
ASensingSphere::ASensingSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));
	RootComponent = SphereCollider;
	BallMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASensingSphere::BeginPlay()
{
	Super::BeginPlay();

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ASensingSphere::OverlapsWithActors);
	SetActorTickEnabled(false);
	GameMode = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode());

	GameMode->SetSphereIsRunning(true);

	Activate();
	
}

// Called every frame
void ASensingSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Scale += FVector (DeltaTime*SpeedScale, DeltaTime*SpeedScale, DeltaTime*SpeedScale);
	
	SetActorScale3D(Scale);

}

void ASensingSphere::Activate()
{
	TurnOnOtherActorCollisions();
	AdjustSphereColliderForWorldType();
	SetActorTickEnabled(true);
	GetWorldTimerManager().SetTimer(KillTimer, this, &ASensingSphere::Kill, 1.5f, false);
}

void ASensingSphere::Kill()
{
	TurnOffOtherActorCollisions();
	GameMode->SetSphereIsRunning(false);
	Destroy();
}

//Set other actors to be be able to overlap with sphere but not block player if moving
void ASensingSphere::TurnOnOtherActorCollisions()
{
	if (GameMode->bIsSpiritWorld)
	{
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{
			APWorldActor* Actor = *PActorItr;

			if (Actor->bCanBeSensed)
			{
				Actor->SetActorEnableCollision(true);

				UStaticMeshComponent* MeshComp = Actor->GetMeshRef();
				if (MeshComp)
				{
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Physical: Got MeshComponent in actor iterator. Set to Query Only"))
				}
				else UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Physical: DID NOT GET MeshComponent in actor iterator."))
			}
		}
	}

	else
	{
		for (TActorIterator<ASWorldActor> SActorItr(GetWorld()); SActorItr; ++SActorItr)
		{
			ASWorldActor* Actor = *SActorItr;
			if (Actor->bCanBeSensed)
			{
				Actor->SetActorEnableCollision(true);

				//So objects can overlap with sensing sphere but not block player if moving
				UStaticMeshComponent* MeshComp = Actor->GetMeshRef();

				if (MeshComp)
				{
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Spirit: Got MeshComponent in actor iterator. Set to Query Only"))
				}
				else UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Spirit: DID NOT GET MeshComponent in actor iterator."))
			}
			
		}
	}
}

//Reset actor
void ASensingSphere::TurnOffOtherActorCollisions()
{
	if (GameMode->bIsSpiritWorld)
	{
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{
			APWorldActor* Actor = *PActorItr;
			if (Actor->bCanBeSensed)
			{
				UStaticMeshComponent* MeshComp = Actor->GetMeshRef();

				if (MeshComp)
				{
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Physical: Got MeshComponent in actor iterator. Set to QueryAndPhysics"))
				}
				else UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Physical: DID NOT GET MeshComponent in actor iterator."))
			}
		}
	}

	else
	{
		for (TActorIterator<ASWorldActor> SActorItr(GetWorld()); SActorItr; ++SActorItr)
		{
			ASWorldActor* Actor = *SActorItr;
			if (Actor->bCanBeSensed)
			{
				Actor->SetActorEnableCollision(false);
				UStaticMeshComponent* MeshComp = Actor->GetMeshRef();

				if (MeshComp)
				{
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Spirit: Got MeshComponent in actor iterator. Set to QueryAndPhysics"))
				}
				else UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Spirit: DID NOT GET MeshComponent in actor iterator."))
			}
		}
	}

}

void ASensingSphere::AdjustSphereColliderForWorldType()
{
	if (GameMode->bIsSpiritWorld)
	{	//Ignore Object type SpiritActor and overlap with PhysicalActor
		SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
		SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	}

	else
	{	//Vice versa
		SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
		SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	}
}

void ASensingSphere::OverlapsWithActors(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(ASWorldActor::StaticClass()) && Cast<ASWorldActor>(OtherActor)->bCanBeSensed)
	{
		OtherActor->SetActorEnableCollision(false);
		Cast<ASWorldActor>(OtherActor)->LightUpActorWhenSensed();
	}
	

	if (OtherActor->IsA(APWorldActor::StaticClass()) && Cast<APWorldActor>(OtherActor)->bCanBeSensed)
	{
			OtherActor->SetActorEnableCollision(false);
		Cast<APWorldActor>(OtherActor)->LightUpActorWhenSensed();

	}

}