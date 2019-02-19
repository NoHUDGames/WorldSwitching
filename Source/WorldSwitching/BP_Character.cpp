// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_Character.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "SpiritTest.h"
#include "Altar.h"
#include "Artifacts.h"

// Sets default values
ABP_Character::ABP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root component for kicking
	KickingRotation = CreateDefaultSubobject<USceneComponent>(TEXT("KickingRotation"));
	KickingRotation->SetupAttachment(RootComponent);
	KickingRotation->Mobility = EComponentMobility::Movable;
	KickingRotation->bVisualizeComponent = true;

	// Create and position a mesh component so we can see where our sphere is 
	// DUMMY mesh, only for prototype. Will exchange when we have animation for kicking
	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FakeLeg"));
	SphereVisual->SetupAttachment(KickingRotation);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->bVisualizeComponent = true;
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
		SphereVisual->Mobility = EComponentMobility::Movable;
		SphereVisual->SetWorldScale3D(FVector(0.2f));
	}

	/// kicking collider, the collision sphere that damages the enemies when kicking
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("FootCollider"));
	BoxCollider->SetupAttachment(SphereVisual);

}

// Called when the game starts or when spawned
void ABP_Character::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::HittingEnemy);

	/// Tests if the player overlaps with an artifact
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::PickingUpArtifacts);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::DeliveringArtifacts);
	
}

// Called every frame
void ABP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveUp", this, &ABP_Character::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABP_Character::MoveRight);
	PlayerInputComponent->BindAction("Kicking", IE_Pressed, this, &ABP_Character::Kicking);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABP_Character::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ABP_Character::StopInteracting);

}

void ABP_Character::MoveUp(float AxisValue)
{//50.f, 0.f, 0.f
	AddMovementInput(FVector(50.f, 0.f, 0.f), AxisValue);
}

void ABP_Character::MoveRight(float AxisValue)
{//0.f, 50.f, 0.f
	AddMovementInput(FVector(0.f, 50.f, 0.f), AxisValue);
}

void ABP_Character::Kicking()
{
	if (!CurrentlyKicking)
	{
		/// Makes it impossible for the player to kick while it's already kicking
		CurrentlyKicking = true;

		if (NumberOfKicks <= 2)
		{
			++NumberOfKicks;
			UE_LOG(LogTemp, Warning, TEXT("Number of kicks: %i"), NumberOfKicks)
			/// Resets the kick after 0.3 seconds
			GetWorldTimerManager().SetTimer(ComboDurationTimer, this, &ABP_Character::ResetKickingCombo, 1.5f, false);
		}
		else
		{
			ResetKickingCombo();
		}

		/// Turns on overlapping with other pawns for the kick box collider
		BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
		BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
		
		/// Rotates the scene component, so the kick kan hit something
		FRotator NewRotation{ 90.f, 0.f ,0.f };
		KickingRotation->AddLocalRotation(NewRotation);

		/// Resets the kick after 0.3 seconds
		GetWorldTimerManager().SetTimer(KickingDurationTimer, this, &ABP_Character::StopKicking, 0.3f, false);

		
	}
	
}

void ABP_Character::StopKicking()
{
	/// Resets all values set in the function Kicking
	FRotator NewRotation{ -90.f,0.f ,0.f };
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	KickingRotation->AddLocalRotation(NewRotation);
	CurrentlyKicking = false;
}

void ABP_Character::ResetKickingCombo()
{
	if (NumberOfKicks != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Resetting kicking combo."))
		NumberOfKicks = 0;
	}
		
}

void ABP_Character::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to interact with something"))

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
}

void ABP_Character::StopInteracting()
{
	UE_LOG(LogTemp, Warning, TEXT("Stops trying to interact with something"))
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
}

void ABP_Character::PickingUpArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	OtherActorForPhysicalTest = OtherActor;

	if (OtherActor->IsA(AArtifacts::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("You're colliding with an artifact."))
		
		++NumberOfHoldingArtifacts;
		AArtifacts* CollidingArtifact = Cast<AArtifacts>(OtherActor);

		CollidingArtifact->PickupFeedback();
	}
}

void ABP_Character::HittingEnemy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	if (OtherActor->IsA(ASpiritTest::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("You are hitting a Spirit Enemy"))

		ASpiritTest* Spirit = Cast<ASpiritTest>(OtherActor);

		if (NumberOfKicks <= 2)
		{
			Spirit->DecrementingLives();
		}
		else
		{
			for (int i{ 0 }; i < 2; ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("Double damage"))
				Spirit->DecrementingLives();
			};
		};
		

		BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

	};
}

void ABP_Character::DeliveringArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AAltar::StaticClass()) && NumberOfHoldingArtifacts > 0 && bIsSpiritWorld == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Colliding with the altar"))

		AAltar* Altar = Cast<AAltar>(OtherActor);

		Altar->ReceivingArtifacts(NumberOfHoldingArtifacts);

		NumberOfHoldingArtifacts = 0;
	}
}


AActor* ABP_Character::GetOtherActorForPhysicalTest()
{
	AActor* temp = OtherActorForPhysicalTest;

	OtherActorForPhysicalTest = nullptr;

	return temp;
}


