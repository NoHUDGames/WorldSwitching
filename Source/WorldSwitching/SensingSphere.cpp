// Fill out your copyright notice in the Description page of Project Settings.

#include "SensingSphere.h"
#include "WorldSwitchingGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Artifacts.h"
#include "S_PickupShield.h"
#include "BP_Character.h"


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
	PlayerPawn = Cast<ABP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));

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
	GetWorldTimerManager().SetTimer(KillTimer, this, &ASensingSphere::Kill, 0.25f, false);
}

void ASensingSphere::Kill()
{
	TurnOffOtherActorCollisions();
	GameMode->SetSphereIsRunning(false);
	Destroy();
}

void ASensingSphere::PrepareArtifactForSensing(AActor* ArtifactActor)
{

	AArtifacts* Artifact = nullptr;

	if (Cast<AArtifacts>(ArtifactActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Prepareartifactforsensing: Object is an artifact"))
			Artifact = Cast<AArtifacts>(ArtifactActor);
	}

	else UE_LOG(LogTemp, Warning, TEXT("Prepareartifactforsensing: Object is not an artifact"))

	

	//Ignore Pawn in case player is overlapping artifact in other world

	if (Artifact)
	{
		Artifact->SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		Artifact->SetActorEnableCollision(true);
	}
	else UE_LOG(LogTemp,Warning,TEXT("SensingSphere: WE DO NOT HAVE ARTIFACT POINTER"))

}

void ASensingSphere::PrepareShieldForSensing(AActor* ShieldActor)
{
	AS_PickupShield* Shield = nullptr;

	if (Cast<AS_PickupShield>(ShieldActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Prepareartifactforsensing: Object is a shield"))
			Shield = Cast<AS_PickupShield>(ShieldActor);
	}

	else UE_LOG(LogTemp, Warning, TEXT("Prepareartifactforsensing: Object is not an artifact"))



		//Ignore Pawn in case player is overlapping artifact in other world

		if (Shield)
		{
			Shield->SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			Shield->SetActorEnableCollision(true);
		}
		else UE_LOG(LogTemp, Warning, TEXT("SensingSphere: WE DO NOT HAVE SHIELD POINTER"))
}


//Set other actors to be be able to overlap with sphere but not block player if moving
void ASensingSphere::TurnOnOtherActorCollisions()
{
	if (GameMode->bIsSpiritWorld)
	{
		PlayerPawn->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{
			APWorldActor* Actor = *PActorItr;

			//Artifacts need a little different treatment because they use a SphereCollider added from blueprint
			if (Cast<AArtifacts>(Actor))
			{
				PrepareArtifactForSensing(Actor);
				continue;
			}

			if (Actor->bCanBeSensed)
			{


				Actor->SetActorEnableCollision(true);

				UStaticMeshComponent* MeshComp = Actor->GetMeshRef();
				if (MeshComp)
				{
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					//UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Physical: Got MeshComponent in actor iterator. Set to Query Only"))
				}
				//else UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Physical: DID NOT GET MeshComponent in actor iterator."))
			}
		}
	}

	else
	{
		PlayerPawn->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
		for (TActorIterator<ASWorldActor> SActorItr(GetWorld()); SActorItr; ++SActorItr)
		{
			ASWorldActor* Actor = *SActorItr;

			if (Cast<AS_PickupShield>(Actor))
			{
				PrepareShieldForSensing(Actor);
				continue;
			}

			if (Actor->bCanBeSensed)
			{
				Actor->SetActorEnableCollision(true);

				//So objects can overlap with sensing sphere but not block player if moving
				UStaticMeshComponent* MeshComp = Actor->GetMeshRef();

				if (MeshComp)
				{
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					//UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Spirit: Got MeshComponent in actor iterator. Set to Query Only"))
				}
				//else UE_LOG(LogTemp, Warning, TEXT("TurnOnOtherActorCollisions/Spirit: DID NOT GET MeshComponent in actor iterator."))
			}
			
		}
	}
}

//Reset actor
void ASensingSphere::TurnOffOtherActorCollisions()
{
	if (GameMode->bIsSpiritWorld)
	{
		PlayerPawn->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

		for (TActorIterator<APWorldActor> PActorItr(GetWorld()); PActorItr; ++PActorItr)
		{
			APWorldActor* Actor = *PActorItr;

			/*
			if (Cast<AArtifacts>(Actor))
			{
				ResetArtifactAfterSensing(Actor);
				continue;
			}
			*/


			if (Actor->bCanBeSensed)
			{
				UStaticMeshComponent* MeshComp = Actor->GetMeshRef();

				if (MeshComp)
				{
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					//UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Physical: Got MeshComponent in actor iterator. Set to QueryAndPhysics"))
				}
				//else UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Physical: DID NOT GET MeshComponent in actor iterator."))
			}
		}
	}

	else
	{
		PlayerPawn->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
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
					//UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Spirit: Got MeshComponent in actor iterator. Set to QueryAndPhysics"))
				}
				//else UE_LOG(LogTemp, Warning, TEXT("TurnOffOtherActorCollisions/Spirit: DID NOT GET MeshComponent in actor iterator."))
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

		//Overlap with Artifacts(Channel GeneralOverlap)
		//SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);


	}

	else
	{	//Vice versa
		SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
		SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		//SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	}
}

void ASensingSphere::OverlapsWithActors(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!GameMode->bIsSpiritWorld)
	{
		if (OtherActor->IsA(ASWorldActor::StaticClass()) && Cast<ASWorldActor>(OtherActor)->bCanBeSensed)
		{
			OtherActor->SetActorEnableCollision(false);

			Cast<ASWorldActor>(OtherActor)->LightUpActorWhenSensed();
		}
	}

	else
	{
		if (OtherActor->IsA(APWorldActor::StaticClass()) && Cast<APWorldActor>(OtherActor)->bCanBeSensed)
		{
			OtherActor->SetActorEnableCollision(false);

			Cast<APWorldActor>(OtherActor)->LightUpActorWhenSensed();;

		}
	}
}