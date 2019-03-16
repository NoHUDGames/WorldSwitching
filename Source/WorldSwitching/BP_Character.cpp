// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_Character.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "SpiritTest.h"
#include "PShamanEnemy.h"
#include "Altar.h"
#include "Artifacts.h"
#include "S_PickupShield.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Components/TimelineComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABP_Character::ABP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root component for kicking
	KickingRotation = CreateDefaultSubobject<USceneComponent>(TEXT("KickingRotation"));
	KickingRotation->SetupAttachment(RootComponent);
	KickingRotation->Mobility = EComponentMobility::Movable;


	// Create and position a mesh component so we can see where our sphere is 
	// DUMMY mesh, only for prototype. Will exchange when we have animation for kicking
	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FakeLeg"));
	SphereVisual->SetupAttachment(KickingRotation);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (SphereVisualAsset.Succeeded())
	{
		/// SphereVisual->bVisualizeComponent = true;
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
		SphereVisual->Mobility = EComponentMobility::Movable;
		SphereVisual->SetWorldScale3D(FVector(0.2f));
	}

	/// kicking collider, the collision sphere that damages the enemies when kicking
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("FootCollider"));
	BoxCollider->SetupAttachment(SphereVisual);

	RespawnLocation = { 0.f, 0.f, 0.f };
	NumberOfHoldingArtifacts = 0;

	/// Sets up the timeline that determines the players ability to kick
	KickingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineForKicking"));

	InterpFunction.BindUFunction(this, FName("KickingTimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnKickingTimelineFinished"));

	PitchOffset = 70.f;
	/// Finished setting up the timeline for kicking movement


	/// Setting up animation variables
	static ConstructorHelpers::FObjectFinder<UAnimSequence> idle_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Main_Character_Turning_Head.Main_Character_Turning_Head'"));
	IdleAnim = idle_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> kicking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Main_Character_Kick.Main_Character_Kick'"));
	KickingAnim = kicking_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> walking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Main_Character_Walk_Cycle.Main_Character_Walk_Cycle'"));
	WalkingAnim = walking_Anim.Object;

	

}

// Called when the game starts or when spawned
void ABP_Character::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::HittingEnemy);

	/// Tests if the player overlaps with an artifact
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::PickingUpArtifacts);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::DeliveringArtifacts);
	
	RespawnLocation = GetActorLocation();

	/// Sets up the BeginPlay values for the kicking timeline
	if (fKickingCurve)
	{
		/// Add the float curve to the timeline and connect it to the interpfunctions delegate
		KickingTimeline->AddInterpFloat(fKickingCurve, InterpFunction, FName("Alpha"));
		//Add our timeline finished function
		KickingTimeline->SetTimelineFinishedFunc(TimelineFinished);

		/// Setting vectors
		StartRotationOfKicking = KickingRotation->GetComponentRotation();
		EndRotationOfKicking = FRotator(StartRotationOfKicking.Pitch + PitchOffset, StartRotationOfKicking.Yaw, StartRotationOfKicking.Roll);

		/// Setting our timeline settings before we start it
		KickingTimeline->SetLooping(false);
		KickingTimeline->SetIgnoreTimeDilation(true);
		
	}
	/// Done setting up the BeginPlay values for the kicking timeline
	GameInstance = Cast<UWorldSwitchingGameInstance>(GetWorld()->GetGameInstance());
	PlayerController = Cast<AOurPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

// Called every frame
void ABP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (isIdleAnimStarted == false && RunningAnimations == IDLE)
	{
	GetMesh()->PlayAnimation(IdleAnim, true);
	isIdleAnimStarted = true;
	isWalkingAnimStarted = false;
	isKickingAnimStarted = false;

	}
	else if (isKickingAnimStarted == false && RunningAnimations == KICKING)
	{
		GetMesh()->PlayAnimation(KickingAnim, false);
		isKickingAnimStarted = true;
		isWalkingAnimStarted = false;
		isIdleAnimStarted = false;
	}
	else if (isWalkingAnimStarted == false && (RunningAnimations == WALKINGFORWARD || RunningAnimations == STRIFING))
	{
		GetMesh()->PlayAnimation(WalkingAnim, true);
		isWalkingAnimStarted = true;
		isIdleAnimStarted = false;
		isKickingAnimStarted = false;
	}
	
	

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
	
	if (AxisValue != 0)
	{
		RunningAnimations = ABP_Character::WALKINGFORWARD;
		UE_LOG(LogTemp, Warning, TEXT("%f"), AxisValue)
	}
	else
	{
		if (RunningAnimations != ABP_Character::KICKING && RunningAnimations != ABP_Character::STRIFING)
		{
			RunningAnimations = ABP_Character::IDLE;
		}
		
	}
	
}

void ABP_Character::MoveRight(float AxisValue)
{//0.f, 50.f, 0.f
	AddMovementInput(FVector(0.f, 50.f, 0.f), AxisValue);
	
	if (AxisValue != 0)
	{
		RunningAnimations = ABP_Character::STRIFING;
		UE_LOG(LogTemp, Warning, TEXT("%f"), AxisValue)
	}
	else
	{
		if (RunningAnimations != ABP_Character::KICKING && RunningAnimations != ABP_Character::WALKINGFORWARD)
		{
			RunningAnimations = ABP_Character::IDLE;
		}

	}
}

void ABP_Character::Kicking()
{
	RunningAnimations = KICKING;

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
		
		/// Rotates the scene component, so the kick can hit something
		// FRotator NewRotation{ 90.f, 0.f ,0.f };
		// KickingRotation->AddLocalRotation(NewRotation);

		/// Starts the timeline
		KickingTimeline->Play();

		/// Resets the kick after 0.3 seconds
		/// GetWorldTimerManager().SetTimer(KickingDurationTimer, this, &ABP_Character::StopKicking, 0.3f, false);

	}
	
}

void ABP_Character::StopKicking()
{
	/// Resets all values set in the function Kicking
	FRotator NewRotation{ -90.f,0.f ,0.f };
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
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

void ABP_Character::KickingTimelineFloatReturn(float value)
{
	KickingRotation->SetRelativeRotation(FMath::Lerp(StartRotationOfKicking, EndRotationOfKicking, value));

}

void ABP_Character::OnKickingTimelineFinished()
{
	
	if (KickingTimeline->GetPlaybackPosition() != 0.0f)
	{
		GLog->Log("REVERSE");
		KickingTimeline->Reverse();
	}
	else
	{
		CurrentlyKicking = false;
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

void ABP_Character::DecrementingLives()
{
	if (Lives > 0)
	{
		--Lives;
	}
}

void ABP_Character::PickingUpArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Sent to GameMode for testing before physical reentry
	OtherActorForPhysicalTest = OtherActor;

	if (Cast<AArtifacts>(OtherActor))
	{
		OtherActor->SetActorEnableCollision(false);
		AArtifacts* PickedUpActor = Cast<AArtifacts>(OtherActor);
		
		UE_LOG(LogTemp, Warning, TEXT("You're colliding with an artifact."))

		++NumberOfHoldingArtifacts;
		GameInstance->RegisterPickedUpArtifact(PickedUpActor->GetArrayIndex());
		PickedUpActor->PickupFeedback();
		

		UE_LOG(LogTemp,Warning, TEXT("We have %i artifacts"), NumberOfHoldingArtifacts)
	}

	else if (Cast<AS_PickupShield>(OtherActor))
	{
		OtherActor->SetActorEnableCollision(false);
		AS_PickupShield* PickedUpActor = Cast<AS_PickupShield>(OtherActor);
		GameInstance->RegisterPickedUpShield(PickedUpActor->GetArrayIndex());
		PickedUpActor->PickupFeedback();
		UE_LOG(LogTemp, Warning, TEXT("You're picking up a SHIELD!"))
	}
	else UE_LOG(LogTemp, Warning, TEXT("ALL CASTS FAILED / NOT PICKUP ACTOR"))
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
			}
		}
	
	}

	else if (OtherActor->IsA(APShamanEnemy::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("You are hitting a Shaman Enemy"))

			APShamanEnemy* Shaman = Cast<APShamanEnemy>(OtherActor);
		if (NumberOfKicks <= 2)
		{
			Shaman->DecrementingLives();
		}
		else
		{
			for (int i{ 0 }; i < 2; ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("Double damage"))
				Shaman->DecrementingLives();
			}
		}
	}

	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}


void ABP_Character::DeathSequence()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathExplosion, GetTransform());

	UWorld* World = GetWorld();

	FVector ArtifactSpawnArea = GetActorLocation();

	int Amount = NumberOfHoldingArtifacts;

	for (int i = 0; i < Amount; ++i)
	{
		ArtifactSpawnArea.X += FMath::RandRange(-200.f, 200.f);
		ArtifactSpawnArea.Y += FMath::RandRange(-200.f, 200.f);

		//if (World)
		World->SpawnActor<AArtifacts>(ArtifactsToSpawn, ArtifactSpawnArea, FRotator(0.f, 0.f, 0.f));
	}

	PlayerController->Artifacts = 0;

	GetWorldTimerManager().SetTimer(DeathSequenceTimer, this, &ABP_Character::RespawnSequence, 3.f, false);
}

void ABP_Character::RespawnSequence()
{
	Lives = 3;
	PlayerController->SetHealth(3);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorLocation(RespawnLocation);
}



void ABP_Character::DeliveringArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AAltar::StaticClass()) && NumberOfHoldingArtifacts > 0 && bIsSpiritWorld == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("ARTIFACTS DELIVERED"))

		Cast<AAltar>(OtherActor)->ReceivingArtifacts(NumberOfHoldingArtifacts);

		NumberOfHoldingArtifacts = 0;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARTIFACTS NOT DELIVERED"))
	}
}


AActor* ABP_Character::GetOtherActorForPhysicalTest()
{
	AActor* temp = OtherActorForPhysicalTest;

	OtherActorForPhysicalTest = nullptr;

	return temp;
}

void ABP_Character::SetRespawnLocation(FVector NewSaveLocation)
{
	RespawnLocation = NewSaveLocation;

}

FVector ABP_Character::GetRespawnLocation()
{
	return RespawnLocation;
}

int ABP_Character::GetLives()
{
	return Lives;
}

int ABP_Character::GetArtifacts()
{
	return NumberOfHoldingArtifacts;
}

void ABP_Character::SetLives(int NewHealth)
{
	Lives = NewHealth;
}
void ABP_Character::SetArtifacts(int NewArtifacts)
{
	NumberOfHoldingArtifacts = NewArtifacts;
}

void ABP_Character::SetbIsSpiritWorld(bool state)
{
	bIsSpiritWorld = state;
}