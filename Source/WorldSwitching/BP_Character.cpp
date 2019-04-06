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
#include "GameFramework/CharacterMovementComponent.h"
#include "WorldSwitchingGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABP_Character::ABP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterHead"));
	Head->SetupAttachment(GetMesh());
	Head->SetMobility(EComponentMobility::Movable);

	Mask = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMask"));
	Mask->SetupAttachment(GetMesh());
	Mask->SetMobility(EComponentMobility::Movable);


	/// kicking collider, the collision sphere that damages the enemies when kicking
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("FootCollider"));
	BoxCollider->SetupAttachment(GetMesh());

	RespawnLocation = { 0.f, 0.f, 0.f };
	NumberOfHoldingArtifacts = 0;


	/// Sets up the timeline for dashing
	DashingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineForDashing"));

	InterpDashingFunction.BindUFunction(this, FName("DashingTimelineFloatReturn"));
	DashingTimelineFinished.BindUFunction(this, FName("OnDashingTimelineFinished"));

	/// Finished setting up the timeline for dashing


	/// Setting up animation variables
	

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> idle_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Player_Idle.Player_Idle'"));
	IdleAnim = idle_Anim.Object;


	static ConstructorHelpers::FObjectFinder<UAnimationAsset> kicking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Player_Kick_1.Player_Kick_1'"));
	KickingAnim = kicking_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> walking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Main_Walk.Main_Walk'"));
	WalkingAnim = walking_Anim.Object;

	
	/// finished setting up animation variables


	/// Sets up the timeline for floating head
	FloatingHeadTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineForFloatingHead"));

	InterpHeadFloatingFunction.BindUFunction(this, FName("FloatingHeadTimelineFloatReturn"));
	HeadFloatingTimelineFinished.BindUFunction(this, FName("OnFloatingHeadTimelineFinished"));

	HeadFloatOffset = 20.f;
	/// finished setting up the timeline for floating head

	/// Sets up the timeline for switching head
	SwitchingHeadTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineForHeadSwitching"));

	InterpHeadSwitchingFunction.BindUFunction(this, FName("SwitchingHeadTimelineFloatReturn"));
	HeadSwitchingTimelineFinished.BindUFunction(this, FName("OnHeadSwitchingTimelineFinished"));

	HeadSwitchingOffset = 50.f;

	
	/// finished setting up the timeline for switching head
}

// Called when the game starts or when spawned
void ABP_Character::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::HittingEnemy);

	/// Tests if the player overlaps with an artifact
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::PickingUpArtifacts);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::DeliveringArtifacts);
	
	GameModeRef = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode());

	RespawnLocation = GetActorLocation();

	BoxCollider->AttachToComponent(GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true),
		FName("KickingRotation"));

	/// Sets up the BeginPlay values for the dashing timeline
	if (fDashingCurve)
	{
		/// Add the float curve to the timeline and connect it to the interpfunctions delegate
		DashingTimeline->AddInterpFloat(fDashingCurve, InterpDashingFunction, FName("Alpha"));
		//Add our timeline finished function
		DashingTimeline->SetTimelineFinishedFunc(DashingTimelineFinished);

		/// Setting our timeline settings before we start it
		DashingTimeline->SetLooping(false);
		DashingTimeline->SetIgnoreTimeDilation(true);

	}
	/// Done setting up the BeginPlay values for the dashing timeline

	/// Setting up BeginPlay values for world switching
	/// Sets up the BeginPlay values for the floating head timeline
	if (fFloatingHeadCurve)
	{
		FloatingHeadTimeline->AddInterpFloat(fFloatingHeadCurve, InterpHeadFloatingFunction, FName("Alpha"));

		FloatingHeadTimeline->SetLooping(true);
		FloatingHeadTimeline->SetIgnoreTimeDilation(true);

		if (bIsSpiritWorld)
		{
			FloatingHeadStartLocation = Head->GetRelativeTransform().GetLocation();
			Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false), FName("HeadSocket"));
		}
		else
		{
			FloatingHeadStartLocation = Mask->GetRelativeTransform().GetLocation();
			Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false), FName("HeadSocket"));
		}

		FloatingHeadGoalLocation = FloatingHeadStartLocation + FVector(0.f, 0.f, HeadFloatOffset);
		
	}
	///Finished setting up the BeginPlay values for the floating head timeline

	/// Sets up the BeginPlay values for the switching head timeline
	if (fHeadSwitchingCurve)
	{
		/// Add the float curve to the timeline and connect it to the interpfunctions delegate
		SwitchingHeadTimeline->AddInterpFloat(fHeadSwitchingCurve, InterpHeadSwitchingFunction, FName("Alpha"));
		//Add our timeline finished function
		SwitchingHeadTimeline->SetTimelineFinishedFunc(HeadSwitchingTimelineFinished);

		/// Setting our timeline settings before we start it
		SwitchingHeadTimeline->SetLooping(false);
		SwitchingHeadTimeline->SetIgnoreTimeDilation(true);
	}
	///Finished setting up the BeginPlay values for the switching head timeline


	if (bIsSpiritWorld)
	{
		HeadSocketLocation = Mask->GetRelativeTransform().GetLocation();
		TrailingHeadLocation = Head->GetRelativeTransform().GetLocation();
	}
	else
	{
		HeadSocketLocation = Head->GetRelativeTransform().GetLocation();
		TrailingHeadLocation = Mask->GetRelativeTransform().GetLocation();
	}

	/// Finished setting up the BeginPlay values for world switching
	

	GameInstance = Cast<UWorldSwitchingGameInstance>(GetWorld()->GetGameInstance());
	PlayerController = Cast<AOurPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));


	if (GetCharacterMovement()->GetMaxSpeed() != NormalSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}

	
}

// Called every frame
void ABP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PlayingAnimations();
	
	FloatingHeadTimeline->Play();

}

void ABP_Character::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	
	if (GetCharacterMovement()->GetMovementName() == "Falling")
	{
		startedFalling = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	}
	else if (GetCharacterMovement()->GetMovementName() == "Walking")
	{
		endedFalling = UGameplayStatics::GetRealTimeSeconds(GetWorld());	

		CalculateFallDuration();
	}
}

void ABP_Character::CalculateFallDuration()
{
	float TimeFalled = endedFalling - startedFalling;

	if (TimeFalled > FallDurationForDeath)
	{
		DeathSequence();
	}
}

void ABP_Character::PlayingAnimations()
{
	
	if (AnimationStarted[0] == false && RunningAnimations == EAnimations::IDLE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spiller av idle"))
		GetMesh()->PlayAnimation(IdleAnim, true);

		ChangingAnimationStarted(0);

	}
	else if (AnimationStarted[1] == false && RunningAnimations == EAnimations::ATTACKING)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spiller av kicking"))
		GetMesh()->PlayAnimation(KickingAnim, false);

		ChangingAnimationStarted(1);
	}
	
	else if (AnimationStarted[2] == false && RunningAnimations == EAnimations::WALKINGFORWARD)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spiller av walking forward"))
		GetMesh()->PlayAnimation(WalkingAnim, true);

		ChangingAnimationStarted(2);
	}
	else if (AnimationStarted[3] == false && RunningAnimations == EAnimations::STRIFING)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spiller av strifing"))
		GetMesh()->PlayAnimation(WalkingAnim, true);

		ChangingAnimationStarted(3);
	}
	else if (AnimationStarted[4] == false && RunningAnimations == EAnimations::DASHING)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spiller av dashing"))
			GetMesh()->PlayAnimation(WalkingAnim, true);

		ChangingAnimationStarted(4);
	}
	
}

void ABP_Character::ChangingAnimationStarted(int index)
{
	AnimationStarted[index] = true;
	for (int i{ 0 }; i < 5; ++i)
	{
		if (i != index)
		{
			AnimationStarted[i] = false;
		}
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
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ABP_Character::Dashing);
	PlayerInputComponent->BindAction("SenseWorld", IE_Pressed, this, &ABP_Character::SenseWorld);

}

void ABP_Character::MoveUp(float AxisValue)
{//50.f, 0.f, 0.f
	AddMovementInput(FVector(50.f, 0.f, 0.f), AxisValue);
	
	MovementAnimationTesting(AxisValue, UKismetMathLibrary::GetForwardVector(GetActorRotation()).Y);
}

void ABP_Character::MoveRight(float AxisValue)
{//0.f, 50.f, 0.f
	AddMovementInput(FVector(0.f, 50.f, 0.f), AxisValue);

	MovementAnimationTesting(AxisValue, UKismetMathLibrary::GetForwardVector(GetActorRotation()).X);
}

void ABP_Character::MovementAnimationTesting(float AxisValue, float ForwardVector)
{
	if (AxisValue != 0 && !CurrentlyKicking && RunningAnimations != EAnimations::DASHING)
	{
		if (ForwardVector < 0.6f && ForwardVector > -0.6f)
		{
			RunningAnimations = EAnimations::WALKINGFORWARD;
		}
		else
		{
			RunningAnimations = EAnimations::STRIFING;
		}

	}
	/// if the player aren't moving forward, aren't kicking and aren't strifing, the IdleAnim runs
	else
	{
		if (RunningAnimations != EAnimations::ATTACKING && GetVelocity().IsZero())
		{
			RunningAnimations = EAnimations::IDLE;
		}

	}
	/// end of if statement that determines what animation should be run
}


void ABP_Character::Kicking()
{
	RunningAnimations = EAnimations::ATTACKING;

	if (!CurrentlyKicking)
	{
		/// Makes it impossible for the player to kick while it's already kicking
		CurrentlyKicking = true;

		if (NumberOfKicks <= 2)
		{
			++NumberOfKicks;
			UE_LOG(LogTemp, Warning, TEXT("Number of kicks: %i"), NumberOfKicks)
			/// Resets the kick after 1.5 seconds
			GetWorldTimerManager().SetTimer(ComboDurationTimer, this, &ABP_Character::ResetKickingCombo, 1.5f, false);
		}
		else
		{
			ResetKickingCombo();
		}

		/// Turns on overlapping with other pawns for the kick box collider
		BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
		BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

		GetWorldTimerManager().SetTimer(KickingDurationTimer, this, &ABP_Character::KickingFinished, 0.875f, false);
	}
	
}

void ABP_Character::ResetKickingCombo()
{
	if (NumberOfKicks != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Resetting kicking combo."))
		NumberOfKicks = 0;
	}	
}

void ABP_Character::KickingFinished()
{
	CurrentlyKicking = false;
	RunningAnimations = EAnimations::IDLE;
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
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
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), TakingDamageCameraShake, GetActorLocation(), 10.f, 1100.f);
	if (GetShields() > 0)
	{
		--NumberOfShields;
	}
	else if (GetLives() > 0)
	{
		--Lives;

		if (Lives <= 0)
		{
			DeathSequence();
		}
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
		PickedUpArtifactsIndexes.Add(PickedUpActor->GetArrayIndex());

		if (PickedUpActor->bKeepTrackOf)
		GameInstance->RegisterPickUp(PickedUpActor->GetArrayIndex(), OtherActor);

		PickedUpActor->PickupFeedback();
		
		UE_LOG(LogTemp,Warning, TEXT("We have %i artifacts"), NumberOfHoldingArtifacts)
	}

	else if (Cast<AS_PickupShield>(OtherActor) && GetShields() < 3)
	{
		OtherActor->SetActorEnableCollision(false);
		AS_PickupShield* PickedUpActor = Cast<AS_PickupShield>(OtherActor);

		if (PickedUpActor->bKeepTrackOf)
		GameInstance->RegisterPickUp(PickedUpActor->GetArrayIndex(), OtherActor);;

		PickedUpActor->PickupFeedback();
		++NumberOfShields;
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
			Spirit->DecrementingLives();
			/*

			///Code meant for combo ability.
			///currently makes the game crash because it adds the enemy twice to the pending kill list, 
			/// depending on if the second life taken is the killing blow or not
			/// won't work on this until we have an animation for it
			for (int i{ 0 }; i < 2; ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("Double damage"))
				Spirit->DecrementingLives();
			}
			*/
			

			
		}
		if (Spirit->Lives <= 0 && isTargetingEnemy == true)
		{
			isTargetingEnemy = false;
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
			Shaman->DecrementingLives();

			/*
			///Code meant for combo ability.
			///currently makes the game crash because it adds the enemy twice to the pending kill list, 
			/// depending on if the second life taken is the killing blow or not
			/// won't work on this until we have an animation for it
			for (int i{ 0 }; i < 2; ++i)
			{

				
				UE_LOG(LogTemp, Warning, TEXT("Double damage"))
				Shaman->DecrementingLives();
			}
			*/
			
		}
		if (Shaman->Lives <= 0 && isTargetingEnemy == true)
		{
			isTargetingEnemy = false;
		}
	}

	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}


void ABP_Character::DeathSequence()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	UWorld* World = GetWorld();

	FVector ArtifactSpawnArea = GetActorLocation();


	for (int i = 0; i < NumberOfHoldingArtifacts; ++i)
	{
		AArtifacts* SpawnedArtifact = nullptr;
		ArtifactSpawnArea.X += FMath::RandRange(-200.f, 200.f);
		ArtifactSpawnArea.Y += FMath::RandRange(-200.f, 200.f);

		SpawnedArtifact = World->SpawnActor<AArtifacts>(ArtifactsToSpawn, ArtifactSpawnArea, FRotator(0.f, 0.f, 0.f));
		if (PickedUpArtifactsIndexes.IsValidIndex(i))
		{
			SpawnedArtifact->SetArrayIndex(PickedUpArtifactsIndexes[i]);
			UE_LOG(LogTemp, Warning, TEXT("PickedUpArtifactsIndex is valid with content %i"), PickedUpArtifactsIndexes[i])
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PickedUpArtifactsIndex is NOT valid"))
			UE_LOG(LogTemp, Warning, TEXT("PickedUpArtifactsIndex contains %i elements"), PickedUpArtifactsIndexes.Num())

		}

		SpawnedArtifact->bKeepTrackOf = true;
		//GameInstance->SetArtifactPickedUp(PickedUpArtifactsIndexes[i]);
		
	}
	NumberOfHoldingArtifacts = 0;
	PickedUpArtifactsIndexes.Empty();

	PlayerController->Artifacts = 0;

	GetWorldTimerManager().SetTimer(DeathSequenceTimer, this, &ABP_Character::RespawnSequence, 3.f, false);
}

void ABP_Character::RespawnSequence()
{
	Lives = 3;
	SetShields(0);
	PlayerController->SetHealth(3);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorLocation(RespawnLocation);
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	isTargetingEnemy = false;
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

void ABP_Character::FloatingHeadTimelineFloatReturn(float value)
{
	if (bIsSpiritWorld)
	{
		Head->SetRelativeLocation(FMath::Lerp(FloatingHeadStartLocation, FloatingHeadGoalLocation, value));
	}
	else
	{
		Mask->SetRelativeLocation(FMath::Lerp(FloatingHeadStartLocation, FloatingHeadGoalLocation, value));
	}
}

void ABP_Character::SwitchingHead()
{
	

	if (bIsSpiritWorld)
	{
		

		Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true)); // resets the head and makes it not snap to the socket
		///SwitchingHeadTimeline->Play();

		Head->SetRelativeLocation(FloatingHeadStartLocation);


		Mask->SetRelativeLocation(HeadSocketLocation);
		Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), FName("HeadSocket"));

	}
	else
	{
		

		Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true)); // resets the mask and makes it not snap to the socket
		///SwitchingHeadTimeline->Reverse();

		Mask->SetRelativeLocation(FloatingHeadStartLocation);
		Head->SetRelativeLocation(HeadSocketLocation);

		Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), FName("HeadSocket"));

		
	}

	
}


/// There's something wrong with this function
void ABP_Character::SwitchingHeadTimelineFloatReturn(float value)
{
	Head->SetRelativeLocation(FMath::Lerp(HeadSocketLocation, TrailingHeadLocation, value));
	Mask->SetRelativeLocation(FMath::Lerp(TrailingHeadLocation, HeadSocketLocation, value));
	
}

void ABP_Character::OnHeadSwitchingTimelineFinished()
{
	if (bIsSpiritWorld)
	{
		///Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true)); // resets the head and makes it not snap to the socket

		Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), FName("HeadSocket"));


	}
	else
	{
		///Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true)); // resets the mask and makes it not snap to the socket

		Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), FName("HeadSocket"));


	}
}

void ABP_Character::Dashing()
{
	RunningAnimations = EAnimations::DASHING;

	if (CurrentlyDashing == false)
	{
		
		/// Setting vectors
		ActorLocation = GetActorLocation();

		FVector CharacterAcceleration = GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal(1.0f);

		GoalLocation = ActorLocation + (CharacterAcceleration * DashingDistance);

		
		ReverseCurrentlyDashing();

		DashingTimeline->PlayFromStart();

	}
	
}

void ABP_Character::DashingTimelineFloatReturn(float value)
{
	SetActorLocation(FMath::Lerp(ActorLocation, GoalLocation, value), true);

}

void ABP_Character::OnDashingTimelineFinished()
{
	RunningAnimations = EAnimations::IDLE;
	GetWorldTimerManager().SetTimer(DashCooldown, this, &ABP_Character::ReverseCurrentlyDashing, 1.0f, false);

}

void ABP_Character::SenseWorld()
{
	if (!GameModeRef->GetSphereIsRunning() && GameModeRef->GetLitUpBySensing() == 0)
	{

		GetWorld()->SpawnActor<ASensingSphere>(SensingSphereToSpawn, GetActorLocation() + FVector(0.f, 0.f,85.f), FRotator(0, 0, 0));
	}
}